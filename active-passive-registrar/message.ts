import {Role} from "./role.js";

export const enum MSG_ID
{
    // Client -> Server
    SetLeaseInformationMSG = 0,

    // Reserved...

    // Server -> Client
    SetRoleMSG = 1000,
}

// Header.
export interface MSG_Header{
    messageLength: number
    messageType: MSG_ID
}

// Client -> Server
export interface SetLeaseInformationMSG{
    identity: string
    leaseName: string
    leaseNamespace: string
}
export function SerializeSetLeaseInformationMSG(message: SetLeaseInformationMSG): Buffer{
    const buffer = Buffer.alloc(1024);
    let offset   = 8;

    // Write body.
    offset = SerializeStringInto(message.identity, buffer, offset);
    offset = SerializeStringInto(message.leaseName, buffer, offset);
    offset = SerializeStringInto(message.leaseNamespace, buffer, offset);

    // Write header.
    buffer.writeInt32LE(offset, 0);
    buffer.writeInt32LE(MSG_ID.SetLeaseInformationMSG, 4);

    // Slice buffer and return.
    const finalBuffer = buffer.subarray(0, offset);
    return finalBuffer;
}
export function DeserializeSetLeaseInformationMSG(buffer: Buffer): SetLeaseInformationMSG{
    let offset = 0;
    let identity = ""
    let leaseName = ""
    let leaseNamespace = ""

    {
        const deserialized = DeserializeStringFrom(buffer, offset);
        identity           = deserialized.value;
        offset             = deserialized.offset;
    }
    {
        const deserialized = DeserializeStringFrom(buffer, offset);
        leaseName          = deserialized.value;
        offset             = deserialized.offset;
    }
    {
        const deserialized = DeserializeStringFrom(buffer, offset);
        leaseNamespace     = deserialized.value;
        offset             = deserialized.offset;
    }
    


    /*
    // get identity
    const identityLength = buffer.readInt32LE(messageIndex);
    messageIndex += 4;

    const identityBuffer = buffer.subarray(messageIndex, messageIndex + identityLength);
    messageIndex += identityLength;
    
    const identity = identityBuffer.toString();

    console.log(identity)

    // get leaseName
    const leaseNameLength = buffer.readInt32LE(messageIndex);
    messageIndex += 4;

    const leaseNameBuffer = buffer.subarray(messageIndex, messageIndex + leaseNameLength);
    messageIndex += leaseNameLength;
    
    const leaseName = leaseNameBuffer.toString();

    console.log(leaseName)

    // get leaseNamespace
    const leaseNamespaceLength = buffer.readInt32LE(messageIndex);
    messageIndex += 4;

    const leaseNamespaceBuffer = buffer.subarray(messageIndex, messageIndex + leaseNamespaceLength);
    messageIndex += leaseNamespaceLength;

    const leaseNamespace = leaseNamespaceBuffer.toString();
    */

    return {
        identity,
        leaseName,
        leaseNamespace
    }
}

// Server -> Client
export interface SetRoleMSG{
    role: Role
}

// Helpers
function SerializeStringInto(serialize: string, buffer: Buffer, offset: number): number
{
    buffer.writeInt32LE(serialize.length, offset);
    offset += 4;

    buffer.write(serialize, offset);
    offset += serialize.length;

    return offset;
}
function DeserializeStringFrom(buffer: Buffer, offset: number): {value: string, offset: number}
{
    const stringLength = buffer.readInt32LE(offset);
    offset += 4;

    const stringBuffer = buffer.subarray(offset, offset + stringLength);
    offset += stringLength;
    
    const value = stringBuffer.toString();

    return {value, offset};
}