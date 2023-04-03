#include <sys/socket.h> // accept
#include <stdio.h> // printf
#include <errno.h> // errno
#include <unistd.h> // fcntl
#include <fcntl.h> // fcntl
#include <stdlib.h> // malloc

#include "TCP_Server.h"
#include "../TCP_Connection/TCP_Connection.h"
#include "../TCP_Connection/init.h"
#include "../Doubly_Linked_List/Doubly_Linked_List_Node.h"
#include "../Doubly_Linked_List/init.h"

int accept_client(TCP_Server* server, TCP_Connection* accepted_client_connection){
    int client_socket = accept(server->socket, NULL, NULL);
    if(client_socket == -1 && errno != EAGAIN && errno != EWOULDBLOCK){
        printf("Failed to accept client socket. Error: %i\n", errno);
        return -1;
    }
    else if(client_socket == -1) return 0; // No client connection to accept.

    if(fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1){
        printf("Failed to set socket to non-blocking.\n");
        return -1;
    }

    init_tcp_connection(accepted_client_connection, client_socket);

    printf("Accepted client\n");

    return 1;
}