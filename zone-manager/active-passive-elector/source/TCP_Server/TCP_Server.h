#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netinet/in.h>

#include "../TCP_Connection/TCP_Connection.h"

#define MAX_CLIENTS 1024

typedef struct TCP_Server{
    int    socket;
    struct sockaddr_in address;

    TCP_Connection client_connections[MAX_CLIENTS];
    int            clients;
} TCP_Server;

#endif