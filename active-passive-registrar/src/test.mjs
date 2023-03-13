import expect from "./expect.mjs";
import k8s from "@kubernetes/client-node";

console.log("testing");

let kc = null;
try
{
    kc = new k8s.KubeConfig();
    kc.loadFromCluster();
    expect(kc, "Kube Config").isNotNull();
}
catch(error)
{
    console.log(`Caught error when initializing Kube Config. Error: ${error}`);
    process.exit(1);
}

console.log(kc)
