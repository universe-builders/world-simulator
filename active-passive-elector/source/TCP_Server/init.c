#include <sys/socket.h> // socket, bind, listen
#include <string.h> // memset
#include <errno.h> // errno
#include <stdio.h> // printf
#include <signal.h> // signal

#include "TCP_Server.h"

void sigpipe_signal_handler(int signo){
    if(signo == SIGPIPE){
        // Do nothing, SIGPIPE means the connection closed on the client side.
        // This will be detected in the write return value and the client will be removed.
        // If this signal wasn't handled, then it will cause an error return for the process.
    }
}

int init_tcp_server(TCP_Server* server, int port){
    signal(SIGPIPE, sigpipe_signal_handler);
    
    server->clients = 0;
    server->client_connections = 0x00;

    server->socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(server->socket == -1){
        printf("Failed to create socket. Error: %i. Return: %i\n", errno, server->socket);
        return -1;
    }

    memset(&server->address, 0, sizeof(server->address));
    server->address.sin_family      = AF_INET;
    server->address.sin_addr.s_addr = htonl(INADDR_ANY);
    server->address.sin_port        = htons(port); 
    if(bind(server->socket, (struct sockaddr*)&server->address, sizeof(server->address)) == -1){
        printf("Failed to bind socket with address. Error: %i\n", errno);
        return -1;
    }
    if(listen(server->socket, 10) == -1){
        printf("Failed to listen to socker. Error: %i\n", errno);
        return -1;
    }

    return 0;
}