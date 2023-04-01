import k8s from "@kubernetes/client-node";

import {Role} from "./role.js"
import expect, {Severity} from "./expect.js";

export default async function renewActiveLease(kubeAPIClient: k8s.CoordinationV1Api, identity: string, role: Role | undefined): Promise<Role | undefined>{
    // Read current lease.
    let lease = null;
    try{
        const response = await kubeAPIClient.readNamespacedLease("distribute-zone-simulation-active", "default");
        if( expect(response.response.statusCode, "Status Code", Severity.WARNING).isGTorEQ(200).isLTorEQ(299).result === false ) return role;

        lease = response.body;
    }
    catch(error){
        console.log(`Caught error when reading current lease.`, error);
        return role;
    }

    // Attempt to renew lease.
    try{
        if( expect(lease.spec, "Lease Spec", Severity.WARNING).result == false ) return role;
        const spec = lease.spec as k8s.V1LeaseSpec;

        // Lease has changed hands.
        const identityOnLeaseIsntMine = spec.holderIdentity === undefined || spec.holderIdentity !== identity;
        if(identityOnLeaseIsntMine){
            return Role.PASSIVE;
        }

        const currentTime = new k8s.V1MicroTime();
        const renewTime   = new k8s.V1MicroTime();
        renewTime.setSeconds(currentTime.getSeconds() + (spec.leaseDurationSeconds as number));
        spec.renewTime    = renewTime;

        const response = await kubeAPIClient.replaceNamespacedLease("distribute-zone-simulation-active", "default", lease, undefined, undefined, undefined, undefined, undefined);
        if(response.response.statusCode !== undefined)
        {
            const statusCode = response.response.statusCode as number;
            if(statusCode >= 200 && statusCode <= 299){
                console.log(`Renewed active : ${identity}`);
                return Role.ACTIVE;
            }
            else{
                console.log("Failed to renew active.");
                return role;
            }
        }
    }
    catch(error){
        console.log(`Caught error when getting lease. Error:`, error);
        return role;
    }

    return role;
}