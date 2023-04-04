/* stdlib */
#include <stdio.h>

/* K8s */
#include <incluster_config.h>
#include <kube_config.h>
#include <apiClient.h>
#include <CoreV1API.h>

apiClient_t* initialize_k8s_core_api_client(void)
{
    char*        basePath  = 0x00;
    sslConfig_t* sslConfig = 0x00;
    list_t*      apiKeys   = 0x00;

    int rc = load_incluster_config(&basePath, &sslConfig, &apiKeys);
    if (rc != 0) {
        printf("Cannot load kubernetes configuration in cluster.\n");
        return 0x00;
    }
    apiClient_t* apiClient = apiClient_create_with_base_path(basePath, sslConfig, apiKeys);
    if (apiClient == 0x00) {
        printf("Cannot create a kubernetes client.\n");
        return 0x00;
    }

    return apiClient;
}