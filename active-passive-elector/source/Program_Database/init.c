#include <stdio.h>

#include <c-net/TCP_Server/init.h>
#include <c-k8s/api_client/initialize_k8s_core_api_client.h>

#include "Program_Database.h"
#include "../Client/init_next_client.h"

int init_Program_Database(Program_Database* db){
    if(init_tcp_server(&db->server, 5001) == -1){
        printf("Failed to init TCP server.\n");
        return -1;
    }

    db->clients           = 0x00;
    db->number_of_clients = 0;

    init_next_client(db);

    db->k8s_api_client = initialize_k8s_core_api_client();
    if(db->k8s_api_client == 0x00){
        printf("Failed to initialize K8s API Client\n");
        return -1;
    }

    return 0;
}