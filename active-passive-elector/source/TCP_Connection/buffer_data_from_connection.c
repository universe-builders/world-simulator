#include <unistd.h> // read
#include <errno.h> // errno
#include "TCP_Connection.h"

int buffer_data_from_connection(TCP_Connection* connection){
    void* buffer = (void*)(connection->recv_buffer + connection->recv_buffered);
    int bytesRemainingInBuffer = MAX_RECV_BUFFER - connection->recv_buffered;
    int bytesRead = read(connection->socket, buffer, bytesRemainingInBuffer);
    if(bytesRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) return 0;
    else{
        connection->recv_buffered += bytesRead;
        return bytesRead;
    }
}