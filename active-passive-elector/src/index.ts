import net from "net";
import k8s from "@kubernetes/client-node";
import {readFile} from "node:fs/promises";

import Connection from "./connection.js";
import { Role, roleToString } from "./role.js";
import {MSG_ID, DeserializeSetLeaseInformationMSG, SerializeSetRoleMSG, SetRoleMSG} from "./message.js";
import transitionToActiveIfAppropriate from "./transitionToActiveIfAppropriate.js";
import renewActiveLease from "./renewActiveLease.js";
import initializeK8sCoordAPIClient from "./initializeK8sCoordAPIClient.js";
import initializeK8sCoreAPIClient from "./initializeK8sCoreAPIClient.js";
import expect from "./expect.js";

async function step(connections: Connection[], hostname: string, kubeCoordAPIClient: k8s.CoordinationV1Api, kubeCoreAPIClient: k8s.CoreV1Api): Promise<void>
{
    // Read and buffer data.
    connections.forEach(
        (conn: Connection)=>{
            if(conn.socket.readableLength < 1) return;

            const buffer: Buffer = conn.socket.read() as Buffer;
            conn.bufferedData = Buffer.concat([conn.bufferedData, buffer]);

            //console.log(`Buffered ${buffer.byteLength} total is now ${conn.bufferedData.byteLength}`)
        }
    )

    // Process messages.
    connections.forEach(
        (conn: Connection)=>{
            while(conn.bufferedData.byteLength > 8)
            {
                // Read message length from header.
                const messageLength = conn.bufferedData.readInt32LE(0);
                if(conn.bufferedData.byteLength < messageLength) break;

                // Read message type from header.
                const messageType = conn.bufferedData.readInt32LE(4);

                // Create buffer containing just message body.
                const messageBuffer = conn.bufferedData.subarray(8, messageLength);

                if(messageType == MSG_ID.SetLeaseInformationMSG)
                {
                    const setLeaseInformationMSG = DeserializeSetLeaseInformationMSG(messageBuffer);

                    // Set data on conn.
                    conn.leaseInformation = 
                        {
                            identity:       setLeaseInformationMSG.identity,
                            leaseName:      setLeaseInformationMSG.leaseName,
                            leaseNamespace: setLeaseInformationMSG.leaseNamespace
                        }

                    // Role is undefined as new lease information received.
                    conn.role = undefined;
                }
                else
                {
                    console.log(`Error: Got incorrect message type ${messageType}.`);
                }

                // Remove message from buffer.
                conn.bufferedData = conn.bufferedData.subarray(messageLength, conn.bufferedData.byteLength);
            }
        }
    )

    // Handle changing and syncing roles.
    connections.forEach(
        async (conn: Connection)=>{
            if(conn.leaseInformation === undefined) return; // Skip connections that don't have any lease info, thus no role to gain.

            // Change roles.
            const prevRole = conn.role;
            switch(conn.role)
            {
                case undefined:
                case Role.PASSIVE:
                    conn.role = await transitionToActiveIfAppropriate(kubeCoordAPIClient, conn.leaseInformation.identity, conn.role);
                    break;
                case Role.ACTIVE:
                    conn.role = await renewActiveLease(kubeCoordAPIClient, conn.leaseInformation.identity, conn.role);
                    break;
            }

            // Handle role changed.
            const roleChanged = conn.role !== prevRole;
            if(roleChanged && conn.role !== undefined)
            {
                // Send client new role.
                try {
                    const message: SetRoleMSG = {role: conn.role as Role};
                    const buffer = SerializeSetRoleMSG(message);
                    conn.socket.write(buffer);
                } catch (error) {
                    conn.role = prevRole; // Revert role as client didn't receive it.
                    console.log(`Caught error when attempting to send client new role.`, error);
                }
                
                // Add label to pod.
                try {
                    const readResponse = await kubeCoreAPIClient.readNamespacedPod(hostname, "default");
                    expect(readResponse.response.statusCode, "Status Code").isGTorEQ(200).isLTorEQ(299);

                    const pod = readResponse.body as k8s.V1Pod;
                    const metadata = pod.metadata as k8s.V1ObjectMeta;
                    expect(metadata, "Metadata");

                    const labels = metadata.labels as { [key: string]: string; };
                    expect(labels, "Labels");

                    labels['role'] = roleToString(conn.role as Role);

                    const replaceResponse = await kubeCoreAPIClient.replaceNamespacedPod(
                        "active-passive-elector-vscode-development-environment", 
                        "default",
                        pod
                    );
                    expect(replaceResponse.response.statusCode, "Status Code").isGTorEQ(200).isLTorEQ(299);
                } catch (error) {
                    conn.role = prevRole; // Revert role as pod didn't get set.
                    console.log(`Caught error when attempting to set role label on pod.`, error);
                }
            } 
        }
    )

    setTimeout(()=>{step(connections, hostname, kubeCoordAPIClient, kubeCoreAPIClient)}, 1000);
}
async function main()
{
    const port = 7070;
    const host = '127.0.0.1';
    const connections: Connection[] = [];
    const kubeCoordAPIClient = initializeK8sCoordAPIClient();
    const kubeCoreAPIClient  = initializeK8sCoreAPIClient();

    const buffer = await readFile("/etc/hostname");
    const hostname = buffer.toString().trim();

    const server = net.createServer();
    server.listen(port, host, () => {
        console.log('TCP Server is running on port ' + port + '.');
    });
    server.on('connection', function(socket) {
        const connection: Connection = 
        {
            socket,
            role: undefined,
            bufferedData: Buffer.from([]),
            leaseInformation: undefined
        }
        connections.push(connection);

        socket.on('close', function() {
            let index = connections.findIndex(function(c) {
                return c.socket.remoteAddress === socket.remoteAddress && c.socket.remotePort === socket.remotePort;
            })
            if (index !== -1) connections.splice(index, 1);
        });
    });
    
    step(connections, hostname, kubeCoordAPIClient, kubeCoreAPIClient);
}
main()

