#include <unistd.h> // read
#include <errno.h> // errno
#include "TCP_Connection.h"

int buffer_data_from_connection(TCP_Connection* connection){
    void* buffer = (void*)(connection->recv_buffer + connection->recv_buffered);
    int bytes_remaining_in_buffer = MAX_RECV_BUFFER - connection->recv_buffered;
    int bytes_read = read(connection->socket, buffer, bytes_remaining_in_buffer);
    if(bytes_read == -1){
        if(errno == EAGAIN || errno == EWOULDBLOCK) return 0; // nothing to read.
        else return -1;
    }
    else{
        connection->recv_buffered += bytes_read;
        return bytes_read;
    }
}