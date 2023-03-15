import net from "net";
import {SerializeSetLeaseInformationMSG, SetLeaseInformationMSG, DeserializeSetRoleMSG, MSG_ID} from "./message.js"

const client = new net.Socket();
const port = 7070;
const host = '127.0.0.1';
const identity = "luigi-" + Math.random()
let bufferedData = Buffer.from([])
console.log(identity)

function sendLeaseInformation()
{
    const message: SetLeaseInformationMSG = {
        identity,
        leaseName: "distribute-zone-simulation-active",
        leaseNamespace: "default"
    }
    const buffer = SerializeSetLeaseInformationMSG(message);

    client.write(buffer);
}

async function step()
{
    while(client.readyState === "open" && bufferedData.byteLength > 8)
    {
        // Read message length from header.
        const messageLength = bufferedData.readInt32LE(0);
        if(bufferedData.byteLength < messageLength) break;

        // Read message type from header.
        const messageType = bufferedData.readInt32LE(4);

        // Create buffer containing just message body.
        const messageBuffer = bufferedData.subarray(8, messageLength);

        if(messageType == MSG_ID.SetRoleMSG)
        {
            const setRoleMSG = DeserializeSetRoleMSG(messageBuffer);
            console.log(setRoleMSG);
        }
        else{
            console.log(`Warning: Got unhandled message type. Message Type: ${messageType}`)
        }

        // Remove message from buffer.
        bufferedData = bufferedData.subarray(messageLength, bufferedData.byteLength);
    }

    setTimeout(step, 1000);
}

client.connect(port, host, function() {
    console.log('Connected');
    
    sendLeaseInformation()
});
client.on('data', function(buffer) {
    bufferedData = Buffer.concat([bufferedData, buffer]);
    //console.log(`Buffered: ${buffer.byteLength}, Total: ${bufferedData.byteLength}`)
});
client.on('close', function() {
    console.log('Connection closed');
    Process.exit(0);
});

step();