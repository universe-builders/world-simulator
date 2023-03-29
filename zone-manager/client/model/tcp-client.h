#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <netinet/in.h>

typedef struct TCP_Client{
    int socket;
    struct sockaddr_in server_address;
} TCP_Client;

#endif