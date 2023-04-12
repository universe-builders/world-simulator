#include <stdio.h>

#include <c-k8s/api_client/initialize_k8s_core_api_client.h>

#include "Program_Database.h"

int init_Program_Database(Program_Database* db){
    db->k8s_api_client = initialize_k8s_core_api_client();
    if(db->k8s_api_client == 0x00){
        printf("Failed to initialize K8s API Client\n");
        return -1;
    }

    return 0;
}