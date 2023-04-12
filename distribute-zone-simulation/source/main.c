#include <stdio.h>
#include <unistd.h>

#include <kubernetes/include/apiClient.h>

#include <c-core/Process_Control/Process_Control.h>
#include <c-core/Process_Control/init.h>
#include <c-k8s/api_client/initialize_k8s_core_api_client.h>
#include <c-net/TCP_Connection/buffer_data_from_connection.h>

#include "Program_Database/Program_Database.h"
#include "Program_Database/init.h"



int main(int argc, char *argv[]){
    init_process_control();

    Program_Database* db = (Program_Database*)( malloc(sizeof(Program_Database)) );
    if(init_Program_Database(db) != 0){
        printf("Failed to init program.\n");
        exit(1);
    }

    while(1){
        int buffered = buffer_data_from_connection(&db->active_passive_elector_client.connection);

        if(buffered > 0){ printf("%d\n", buffered); }

        if(process_control.should_exit){
            break;
        }
    }

    return 0;
}
