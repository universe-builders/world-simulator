#include "model.h"

#include <sys/socket.h> // accept
#include <stdio.h> // printf
#include <errno.h> // errno

int accept_clients(TCP_Server* server){
    while(1){
        int client_socket = accept(server->socket, NULL, NULL);
        if(client_socket == -1 && errno != EAGAIN && errno != EWOULDBLOCK){
            printf("Failed to accept client socket. Error: %i\n", errno);
            return -1;
        }
        else if(client_socket == -1) break;

        server->client_socket[server->clients] = client_socket;
        server->clients += 1;
    }

    return 0;
}