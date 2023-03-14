import net from "net";
import Connection from "./connection.js";
//import { Role } from "./role.js";
import {MSG_ID, DeserializeSetLeaseInformationMSG} from "./message.js";

async function step(server: net.Server, connections: Connection[])
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
                    console.log(setLeaseInformationMSG)
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

    setTimeout(step.bind(null, server, connections), 5000);
}
async function main()
{
    const port = 7070;
    const host = '127.0.0.1';
    const connections: Connection[] = [];

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
    });

    step(server, connections);
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