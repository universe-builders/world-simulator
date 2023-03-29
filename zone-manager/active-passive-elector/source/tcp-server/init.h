#ifndef TCP_SERVER_INIT_H
#define TCP_SERVER_INIT_H

typedef struct TCP_Server TCP_Server;

/**
 * @return 0 for success, -1 for error
*/
int init_tcp_server(TCP_Server* to_init, int port);

#endif