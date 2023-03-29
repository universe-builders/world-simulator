#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netinet/in.h>

typedef struct TCP_Server{
    int socket;
    struct sockaddr_in address;
    int connections[1024];
} TCP_Server;

#endif