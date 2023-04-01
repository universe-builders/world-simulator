#include "TCP_Connection.h"

void init_tcp_connection(TCP_Connection* to_init, int socket){
    to_init->socket = socket;
    to_init->recv_buffered = 0;
}