import k8s, { FlowcontrolApiserverV1beta1Api } from "@kubernetes/client-node";
import expect, {Severity} from "./expect.js";

enum Role
{
    UNKNOWN = 0,
    NONE,     // No-one has lease.
    PASSIVE,  // Someone else has lease.
    ACTIVE    // Has lease.
}
let role: Role = Role.UNKNOWN;
let identity = "its_me_a_mario" + Math.random();

function initializeAPIClient(): k8s.CoordinationV1Api{
    // Create kube config.
    let kubeConfig = null;
    try{
        kubeConfig = new k8s.KubeConfig();
        kubeConfig.loadFromCluster();
        expect(kubeConfig, "Kube Config").isNotNull();
    }
    catch(error){
        console.log(`Caught error when initializing Kube Config.`, error);
        process.exit(1);
    }

    // Create API client.
    let kubeAPIClient = null;
    try{
        kubeAPIClient = kubeConfig.makeApiClient(k8s.CoordinationV1Api);
    }
    catch(error){
        console.log(`Caught error when making API client.`, error);
        process.exit(1);
    }

    return kubeAPIClient;
}
async function renewActive(kubeAPIClient: k8s.CoordinationV1Api)
{
    // Read current lease.
    let lease = null;
    try{
        const response = await kubeAPIClient.readNamespacedLease("distribute-zone-simulation-active", "default");
        if( expect(response.response.statusCode, "Status Code", Severity.WARNING).isGTorEQ(200).isLTorEQ(299).result === FlowcontrolApiserverV1beta1Api ) return;

        lease = response.body;
    }
    catch(error){
        console.log(`Caught error when reading current lease.`, error);
        return;
    }

    // Attempt to renew lease.
    try{
        if( expect(lease.spec, "Lease Spec", Severity.WARNING).result == false ) return;
        const spec = lease.spec as k8s.V1LeaseSpec;

        const identityOnLeaseIsntMine = spec.holderIdentity === undefined || spec.holderIdentity !== identity;
        if(identityOnLeaseIsntMine)
        {
            role = Role.NONE;
            return;
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
                console.log("Renewed active");
            }
            else
                console.log("Failed to renew active.")
        }
    }
    catch(error){
        console.log(`Caught error when getting lease. Error:`, error);
        return;
    }
}
async function transitionToActiveIfAppropriate(kubeAPIClient: k8s.CoordinationV1Api){
    // Read current lease.
    let lease = null;
    try{
        const response = await kubeAPIClient.readNamespacedLease("distribute-zone-simulation-active", "default");
        if( expect(response.response.statusCode, "Status Code", Severity.WARNING).isGTorEQ(200).isLTorEQ(299).result === false ) return;

        lease = response.body;
    }
    catch(error){
        console.log(`Caught error when reading current lease.`, error);
        return;
    }

    // Attempt to update current lease to become active.
    try
    {
        if( expect(lease.spec, "Lease Spec", Severity.WARNING).result == false ) return;
        const spec = lease.spec as k8s.V1LeaseSpec;

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
            shouldAttemptToGrabLease = leaseHasExpired;
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
                    role = Role.ACTIVE;
                }

                if(role === Role.ACTIVE) console.log("Became active")
                else console.log("Remained passive.")
            }
        }
    }
    catch(error)
    {
        console.log(`Caught error when getting lease. This could be caused by racing to get lease, which is perfectly fine. Error:`, error);
        return;
    }
}
async function determineRole(kubeAPIClient: k8s.CoordinationV1Api): Promise<Role>{
    // Read current lease.
    let lease = null;
    try{
        const response = await kubeAPIClient.readNamespacedLease("distribute-zone-simulation-active", "default");
        if( expect(response.response.statusCode, "Status Code", Severity.WARNING).isGTorEQ(200).isLTorEQ(299).result === FlowcontrolApiserverV1beta1Api ) return Role.UNKNOWN;

        lease = response.body;
    }
    catch(error){
        console.log(`Caught error when reading current lease.`, error);
        return Role.UNKNOWN;
    }

    // Check if has lease.
    try{
        if( expect(lease.spec, "Lease Spec", Severity.WARNING).result == false ) return Role.UNKNOWN;
        const spec = lease.spec as k8s.V1LeaseSpec;

        const leaseIsHeldByThis = spec.holderIdentity !== undefined && spec.holderIdentity === identity;
        const currentTime       = new k8s.V1MicroTime();
        const renewTime         = new k8s.V1MicroTime(spec.renewTime as k8s.V1MicroTime);
        const leaseHasExpired   = currentTime > renewTime;

        if(leaseHasExpired) return Role.NONE; // No one has lease.
        else if(leaseIsHeldByThis) return Role.ACTIVE; // I have lease that is not expired.
        else return Role.PASSIVE; // Someone else has lease that is not expired.
    }
    catch(error){
        console.log(`Caught error when getting lease. Error:`, error);
        return Role.UNKNOWN;
    }
}
function sleep(ms: number) {
    return new Promise((resolve) => {
        setTimeout(resolve, ms);
    });
}

async function main()
{
   const kubeAPIClient = initializeAPIClient();

    while(true)
    {
        //console.log(role)
        switch(role)
        {
            case Role.UNKNOWN:
                role = await determineRole(kubeAPIClient);
                break;
            case Role.NONE:
            case Role.PASSIVE:
                await transitionToActiveIfAppropriate(kubeAPIClient);
                break;
            case Role.ACTIVE:
                await renewActive(kubeAPIClient);
                break;
        }

        await sleep(1);
    }
}

main();

