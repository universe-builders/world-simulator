#ifndef TCP_CONNECTION_INIT_H
#define TCP_CONNECTION_INIT_H

typedef struct TCP_Connection TCP_Connection;

void init_tcp_connection(TCP_Connection* to_init, int socket);

#endif