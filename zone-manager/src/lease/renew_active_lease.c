/* K8s */
#include <CoreV1API.h>
#include <apiClient.h>
#include <CoordinationV1API.h>

/* zone-manager */
#include "../role/role.h"

Role renew_active_lease(apiClient_t* apiClient)
{
    v1_lease_t* lease = CoordinationV1API_readNamespacedLease(
        apiClient, 
        "distribute-zone-simulation-active", 
        "default", 
        NULL
    );

    lease->spec->holder_identity = "c-client-baby-2";

    v1_lease_t* replacedLease = CoordinationV1API_replaceNamespacedLease(
        apiClient,
        "distribute-zone-simulation-active",
        "default", 
        lease, 
        NULL, NULL, NULL, NULL
    );

    printf("Name: %s\n", replacedLease->metadata->name);
    printf("holder-identity: %s\n", replacedLease->spec->holder_identity);

    return ACTIVE;
}