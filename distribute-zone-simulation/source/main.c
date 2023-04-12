#include <stdio.h>
#include <unistd.h>

#include <kubernetes/include/apiClient.h>

#include <c-core/Process_Control/Process_Control.h>
#include <c-core/Process_Control/init.h>
#include <c-k8s/api_client/initialize_k8s_core_api_client.h>

#include "Program_Database/Program_Database.h"
#include "Program_Database/init.h"


int main(int argc, char *argv[]){
    init_process_control();

    Program_Database* db = (Program_Database*)( malloc(sizeof(Program_Database)) );
    if(init_Program_Database(db) != 0){
        printf("Failed to init program.\n");
        exit(1);
    }

    printf("Looping\n");

    while(1){
        if(process_control.should_exit){
            break;
        }
    }

    return 0;
}
