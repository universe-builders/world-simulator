import k8s from "@kubernetes/client-node";
import expect from "./expect.js";

export default function initializeK8sCoordAPIClient(): k8s.CoordinationV1Api{
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