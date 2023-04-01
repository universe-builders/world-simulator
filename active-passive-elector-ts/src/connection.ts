import net from "net";
import {Role} from "./role.js";

export default interface Connection
{
    socket: net.Socket
    role: Role | undefined
    bufferedData: Buffer
    leaseInformation: {
        identity: string
        leaseName: string
        leaseNamespace: string
    } | undefined
}