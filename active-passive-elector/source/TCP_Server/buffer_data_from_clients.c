#include <stdio.h> //printf
#include "TCP_Server.h"
#include "../TCP_Connection/TCP_Connection.h"
#include "../TCP_Connection/buffer_data_from_connection.h"

void buffer_data_from_clients(TCP_Server* server){    
    int clients_read = 0;
    for(int client_index = 0; clients_read < server->clients && client_index < MAX_CLIENTS; ++client_index){
        TCP_Connection* client_connection = server->client_connections + client_index;
        if(client_connection->socket == 0) continue;

        int ret = buffer_data_from_connection(client_connection);
        printf("Buffered: %i\n", ret);

        if(ret == -1){
            // close connection
            client_connection->socket = 0;
            printf("Client disconnected\n");
        }
    }
}