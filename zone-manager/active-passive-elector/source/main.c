#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include "process-control/model.h"
#include "process-control/init.h"
#include "tcp-server/model.h"
#include "tcp-server/init.h"
#include "tcp-server/accept-clients.h"

int main(int argc, char *argv[]){
    init_process_control();
    
    TCP_Server server;
    if(init_tcp_server(&server, 5000) == -1){
        printf("Failed to init TCP server.\n");
    }

    while(1){
        if(accept_clients(&server) == -1){
            printf("Error when accepting clients. Error: %i\n", errno);
        }

        int clients_read = 0;
        for(int client_index = 0; clients_read < server.clients && client_index < MAX_CLIENTS; ++client_index){
            int client_socket = server.client_socket[client_index];
            if(client_socket == 0) continue;

            // peek at data
            
            clients_read += 1;
        }
        

        /*
        const char* message = "Hello, Net!\n";
        for(int client_index = 0; client_index < server.clients; ++client_index){
            int client_socket = server.client_socket[client_index];
            if(write(client_socket, message, strlen(message)) == -1){
                close(client_socket);
                server.client_socket[client_index] = 0;
                server.clients -= 1;
            }
        }
        */
        
        //close(client_socket);

        if(process_control.should_exit){
            close(server.socket);
            exit(0);
        }
        else sleep(1);
    }

    return 0;
}