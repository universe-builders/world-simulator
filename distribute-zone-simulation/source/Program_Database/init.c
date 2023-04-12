#include <stdio.h>

#include <c-k8s/api_client/initialize_k8s_core_api_client.h>
#include <c-net/TCP_Connection/connect_to_server.h>

#include "Program_Database.h"

int init_k8s(Program_Database* db){
    db->k8s_api_client = initialize_k8s_core_api_client();
    if(db->k8s_api_client == 0x00){
        printf("Failed to initialize K8s API Client\n");
        return -1;
    }

    return 0;
}

int init_active_passive_elector_connection(Program_Database* db){
    if(connect_to_server(&db->active_passive_elector_client.connection, "127.0.0.1", 5001) == -1){
        printf("Failed to connect to active-passive-elector server.\n");
        return -1;
    }

    return 0;
}

int init_Program_Database(Program_Database* db){
    if(init_k8s(db) == -1) return -1;
    if(init_active_passive_elector_connection(db) == -1) return -1;

    return 0;
}