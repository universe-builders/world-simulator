#pragma once

#include <netinet/in.h> // sockaddr_in

#include "../TCP_Connection/TCP_Connection.h"
#include "../Linked_List/Linked_List_Node.h"

typedef struct TCP_Server{
    int    socket;
    struct sockaddr_in address;

    int              clients;            // The number of client_connections.
    Linked_List_Node client_connections;
    //TCP_Connection client_connections[MAX_CLIENTS];
    
} TCP_Server;
