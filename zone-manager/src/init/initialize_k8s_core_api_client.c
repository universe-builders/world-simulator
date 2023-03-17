/* stdlib */
#include <stdio.h>

/* K8s */
#include <incluster_config.h>
#include <kube_config.h>
#include <apiClient.h>
#include <CoreV1API.h>

apiClient_t* initialize_k8s_core_api_client(void)
{
    char*        basePath  = NULL;
    sslConfig_t* sslConfig = NULL;
    list_t*      apiKeys   = NULL;

    int rc = load_incluster_config(&basePath, &sslConfig, &apiKeys);
    if (rc != 0) {
        printf("Cannot load kubernetes configuration in cluster.\n");
        return NULL;
    }
    apiClient_t* apiClient = apiClient_create_with_base_path(basePath, sslConfig, apiKeys);
    if (apiClient == NULL) {
        printf("Cannot create a kubernetes client.\n");
        return NULL;
    }

    return apiClient;
}