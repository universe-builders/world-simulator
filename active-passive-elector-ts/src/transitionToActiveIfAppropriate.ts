import k8s from "@kubernetes/client-node";

import {Role} from "./role.js"
import expect, {Severity} from "./expect.js";

export default async function transitionToActiveIfAppropriate(kubeAPIClient: k8s.CoordinationV1Api, identity: string, role: Role | undefined): Promise<Role | undefined>{
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

    // Attempt to update current lease to become active.
    try
    {
        if( expect(lease.spec, "Lease Spec", Severity.WARNING).result == false ) return role;
        const spec = lease.spec as k8s.V1LeaseSpec;

        // Lease is held by this, transition to active.
        const leaseIsHeldByThis = spec.holderIdentity !== undefined && spec.holderIdentity === identity;
        if(leaseIsHeldByThis) return Role.ACTIVE;

        // Determine if should attempt to grab the lease.
        let shouldAttemptToGrabLease = false;
        if(spec.acquireTime === undefined) shouldAttemptToGrabLease = true; // No one has grabbed lease yet.
        else if(spec.holderIdentity === undefined) shouldAttemptToGrabLease = true; // No one has grabbed lease yet.
        else if(spec.renewTime === undefined) shouldAttemptToGrabLease = true; // No one has grabbed lease yet.
        else // Lease has been grabbed, see if it has expired.
        {
            const currentTime        = new k8s.V1MicroTime();
            const renewTime          = new k8s.V1MicroTime(spec.renewTime);
            const leaseHasExpired    = currentTime > renewTime;

            if(leaseHasExpired)
                shouldAttemptToGrabLease = leaseHasExpired;
            else{
                console.log(`Remained passive : ${identity}`)
                return Role.PASSIVE;
            }

        }

        // Attempt to grab the lease.
        if(shouldAttemptToGrabLease)
        {
            console.log("Attempting to become active by grabbing lease.");
            const spec = lease.spec as k8s.V1LeaseSpec;

            const currentTime = new k8s.V1MicroTime();
            const renewTime   = new k8s.V1MicroTime();
            renewTime.setSeconds(currentTime.getSeconds() + (spec.leaseDurationSeconds as number));

            spec.holderIdentity   = identity;
            spec.acquireTime      = currentTime;
            spec.leaseTransitions = spec.leaseTransitions === undefined ? 0 : spec.leaseTransitions + 1;
            spec.renewTime        = renewTime;

            const response = await kubeAPIClient.replaceNamespacedLease("distribute-zone-simulation-active", "default", lease, undefined, undefined, undefined, undefined, undefined);
            
            if(response.response.statusCode !== undefined)
            {
                const statusCode = response.response.statusCode as number;
                if(statusCode >= 200 && statusCode <= 299){
                    console.log(`Became active : ${identity}`)
                    return Role.ACTIVE;
                }
                else {
                    console.log(`Remained passive : ${identity}`)
                    return Role.PASSIVE;
                }
            }
        }
    }
    catch(error: any)
    {
        if(error && error.statusCode && error.statusCode === 409){
            console.log(`Conflict in getting lease. Identity: ${identity}.`);
            return Role.PASSIVE;
        }

        console.log(`Caught error when getting lease. Error:`, error);
        return role;
    }

    return role;
}