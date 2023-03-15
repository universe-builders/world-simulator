import {readFile} from "node:fs/promises";

import initializeK8sCoreAPIClient from "./initializeK8sCoreAPIClient.js";

async function main()
{
    const buffer = await readFile("/etc/hostname");
    const hostname = buffer.toString().trim();

    const kubeAPIClient = initializeK8sCoreAPIClient();
    
    // Read current lease.
    //let lease = null;
    try{
        const response = await kubeAPIClient.readNamespacedPod(hostname, "default");
        const pod = response.body;

        if(pod.metadata === undefined || pod.metadata.labels === undefined) return;

        pod.metadata.labels['test2'] = 'testing';

        const res = await kubeAPIClient.replaceNamespacedPod(
            "active-passive-elector-vscode-development-environment", 
            "default",
            pod
            );
        console.log(res);
    }
    catch(error){
        console.log(`Caught error when reading current lease.`, error);
    }
}
main()