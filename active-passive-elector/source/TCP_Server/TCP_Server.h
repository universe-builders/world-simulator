#pragma once

#include <netinet/in.h> // sockaddr_in

typedef struct Doubly_Linked_List_Node Doubly_Linked_List_Node;

typedef struct TCP_Server{
    int    socket;
    struct sockaddr_in address;
} TCP_Server;
