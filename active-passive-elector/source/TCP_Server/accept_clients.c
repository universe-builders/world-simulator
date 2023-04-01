#include <sys/socket.h> // accept
#include <stdio.h> // printf
#include <errno.h> // errno
#include <unistd.h> // fcntl
#include <fcntl.h> // fcntl

#include "TCP_Server.h"
#include "../TCP_Connection/init.h"

int accept_clients(TCP_Server* server){
    while(1){
        int client_socket = accept(server->socket, NULL, NULL);
        if(client_socket == -1 && errno != EAGAIN && errno != EWOULDBLOCK){
            printf("Failed to accept client socket. Error: %i\n", errno);
            return -1;
        }
        else if(client_socket == -1) break;

        if(fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1){
            printf("Failed to set socket to non-blocking.\n");
            return -1;
        }
        init_tcp_connection(&server->client_connections[server->clients], client_socket);
        server->clients += 1;
        printf("Accepted client\n");
    }

    return 0;
}