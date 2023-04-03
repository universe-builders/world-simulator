#pragma once

#include <netinet/in.h> // sockaddr_in

typedef struct Doubly_Linked_List_Node Doubly_Linked_List_Node;

typedef struct TCP_Server{
    int    socket;
    struct sockaddr_in address;

    int                      clients;            // The number of client_connections.
    Doubly_Linked_List_Node* client_connections;
} TCP_Server;
