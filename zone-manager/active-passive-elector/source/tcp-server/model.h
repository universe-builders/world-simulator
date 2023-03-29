#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netinet/in.h>

#define MAX_CLIENTS

typedef struct TCP_Server{
    int    socket;
    struct sockaddr_in address;
    int    client_socket[MAX_CLIENTS];
    int    clients;
} TCP_Server;

#endif