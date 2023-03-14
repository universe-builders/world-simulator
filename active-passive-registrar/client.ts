import net from "net";
import {SerializeSetLeaseInformationMSG, SetLeaseInformationMSG} from "./message.js"

const client = new net.Socket();
const port = 7070;
const host = '127.0.0.1';

function sendDataStep()
{
    const message: SetLeaseInformationMSG = {
        identity: "My-identity",
        leaseName: "distribute-zone-simulation-active",
        leaseNamespace: "default"
    }
    const buffer = SerializeSetLeaseInformationMSG(message);

    client.write(buffer);

    console.log(`sent ${buffer.byteLength}`)

    setTimeout(sendDataStep, 3000);
}

client.connect(port, host, function() {
    console.log('Connected');
    
    sendDataStep()
});
client.on('data', function(data) {
    console.log('Server Says : ' + data);
});
client.on('close', function() {
    console.log('Connection closed');
});