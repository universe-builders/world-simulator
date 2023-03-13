import expect from "./expect.mjs";
import k8s from "@kubernetes/client-node";

async function main()
{
    // Create kube config.
    let kc = null;
    try{
        kc = new k8s.KubeConfig();
        kc.loadFromCluster();
        expect(kc, "Kube Config").isNotNull();
    }
    catch(error){
        console.log(`Caught error when initializing Kube Config.`, error);
        process.exit(1);
    }

    // Create API client.
    let k8sAPI = null;
    try{
        k8sAPI = kc.makeApiClient(k8s.CoordinationV1Api);
    }
    catch(error){
        console.log(`Caught error when making API client.`, error);
        process.exit(1);
    }
    
    // Read current lease.
    let lease = null;
    try{
        const response = await k8sAPI.readNamespacedLease("distribute-zone-simulation-active", "default");
        expect(response.response.statusCode).isGTorEQ(200).isLTorEQ(299);

        lease = response.body;
    }
    catch(error){
        console.log(`Caught error when reading current lease.`, error);
        process.exit(1);
    }

    // Attempt to update current lease to become active.
    try
    {
        lease.spec.holderIdentity = "lalala";

        const response = await k8sAPI.replaceNamespacedLease("distribute-zone-simulation-active", "default", lease, undefined, undefined, undefined, undefined, undefined);
        
        expect(response.response.statusCode, "Status Code").isGTorEQ(200).isLTorEQ(299);
        
        console.log(response.response.statusCode)
       
       
    }
    catch(error)
    {
        console.log(`Caught error when patching. Error:`, error);
        process.exit(1);
    }
}

main();