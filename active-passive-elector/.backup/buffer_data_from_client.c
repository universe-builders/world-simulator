#include <stdio.h> // printf

#include "TCP_Server.h"
#include "../TCP_Connection/TCP_Connection.h"
#include "../TCP_Connection/buffer_data_from_connection.h"

void buffer_data_from_client(TCP_Server* server, TCP_Connection* client_connection){
    int ret = buffer_data_from_connection(client_connection);
    printf("Buffered: %i\n", ret);

    if(ret == -1){
        // close connection
        //client_connection->socket = 0;
        printf("Client disconnected\n");
    }
}