/* K8s */
#include <CoreV1API.h>
#include <apiClient.h>
#include <CoordinationV1API.h>

v1_lease_t* get_lease(apiClient_t* api_client, char* lease_name, char* lease_namespace){
    v1_lease_t* lease = CoordinationV1API_readNamespacedLease(
        api_client, 
        (char*)lease_name, 
        (char*)lease_namespace, 
        0x00
    );
    return lease;
}