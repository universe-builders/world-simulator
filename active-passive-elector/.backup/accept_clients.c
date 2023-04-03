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

        TCP_Connection* client_connection = malloc(sizeof(TCP_Connection));
        init_tcp_connection(client_connection, client_socket);

        Doubly_Linked_List_Node* client = malloc(sizeof(Doubly_Linked_List_Node));

        if(server->clients == 0){
            init_Doubly_Linked_List_Node(client, client_connection, 0x00, 0x00);
            server->client_connections = client;
        } else{
            // Add node to front of list for O(1) insertion.
            init_Doubly_Linked_List_Node(client, client_connection, 0x00, 0x00);
            client->next                     = server->client_connections;
            server->client_connections->prev = client;
            server->client_connections       = client;
        }

        server->clients += 1;
        printf("Accepted client\n");
    }

    return 0;
}