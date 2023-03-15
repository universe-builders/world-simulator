import net from "net";
import k8s from "@kubernetes/client-node";

import Connection from "./connection.js";
import { Role } from "./role.js";
import {MSG_ID, DeserializeSetLeaseInformationMSG, SerializeSetRoleMSG, SetRoleMSG} from "./message.js";
import transitionToActiveIfAppropriate from "./transitionToActiveIfAppropriate.js";
import renewActiveLease from "./renewActiveLease.js";
import initializeK8sAPIClient from "./initializeK8sAPIClient.js";

async function step(server: net.Server, connections: Connection[], kubeAPIClient: k8s.CoordinationV1Api)
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
                    conn.role = await transitionToActiveIfAppropriate(kubeAPIClient, conn.leaseInformation.identity, conn.role);
                    break;
                case Role.ACTIVE:
                    conn.role = await renewActiveLease(kubeAPIClient, conn.leaseInformation.identity, conn.role);
                    break;
            }

            // Send client new role.
            const roleChanged = conn.role !== prevRole;
            if(roleChanged && conn.role !== undefined)
            {
                const message: SetRoleMSG = {role: conn.role as Role};
                const buffer = SerializeSetRoleMSG(message);
                conn.socket.write(buffer);
            }
        }
    )

    setTimeout(step.bind(null, server, connections, kubeAPIClient), 5000);
}
async function main()
{
    const port = 7070;
    const host = '127.0.0.1';
    const connections: Connection[] = [];
    const kubeAPIClient = initializeK8sAPIClient();

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
    
    step(server, connections, kubeAPIClient);
}
main()



/*
server.on('connection', function(socket) {
    console.log('CONNECTED: ' + socket.remoteAddress + ':' + socket.remotePort);

    const connection: Connection = 
        {
            socket,
            role: undefined,
            leaseInformation: undefined
        }
    connections.push(connection);

    socket.readableLength

*/
    /*
    socket.on('data', (data: Buffer) => {
        
        if(data.byteLength <= 4) return;


        console.log('DATA ' + socket.remoteAddress + ': ' + data);
    });
    */


    /*
    
    // Add a 'close' event handler to this instance of socket
    sock.on('close', function() {
        let index = sockets.findIndex(function(o) {
            return o.remoteAddress === sock.remoteAddress && o.remotePort === sock.remotePort;
        })
        if (index !== -1) sockets.splice(index, 1);
        console.log('CLOSED: ' + sock.remoteAddress + ' ' + sock.remotePort);
    });

    sockets.forEach(function(sock) {
            sock.write(sock.remoteAddress + ':' + sock.remotePort + " said " + data + '\n');
        });
   
});
 */