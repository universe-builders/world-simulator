#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> // memcpy
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include "Process_Control/Process_Control.h"
#include "Process_Control/init.h"
#include "TCP_Server/TCP_Server.h"
#include "TCP_Server/init.h"
#include "TCP_Server/accept_clients.h"
#include "TCP_Server/buffer_data_from_client.h"
#include "Linked_List/Linked_List_Node.h"
#include "messages/Message_Header.h"
#include "messages/message_types.h"
#include "messages/Set_Lease_Info_Message/Set_Lease_Info_Message.h"
#include "messages/Set_Lease_Info_Message/deserialize.h"
#include "messages/Set_Role_Message/Set_Role_Message.h"
#include "messages/Set_Role_Message/deserialize.h"

typedef struct Program_Database{
    TCP_Server       server;       //
    Linked_List_Node client_roles; //  
} Program_Database;

int main(int argc, char *argv[]){
    init_process_control();

    Program_Database db;

    // Init.
    if(init_tcp_server(&db.server, 5000) == -1){
        printf("Failed to init TCP server.\n");
    }

    while(1){
        if(accept_clients(&db.server) == -1){
            printf("Error when accepting clients. Error: %i\n", errno);
        }

        if(db.server.clients > 0){
            Linked_List_Node* client = db.server.client_connections;
            while(client != 0x00){
                TCP_Connection* client_connection = (TCP_Connection*)( client->data );

                buffer_data_from_client(&db.server, client_connection);

                int bytes_processed = 0;
                while(bytes_processed < client_connection->recv_buffered){
                    char* buffer = client_connection->recv_buffer + bytes_processed;
                    Message_Header* message_header = (Message_Header*)buffer;
                    if(message_header->message_length > client_connection->recv_buffered) break;
                
                    if(message_header->message_type == SET_LEASE_INFO_MSG_ID){
                        Set_Lease_Info_Message message;
                        deserialize_Set_Lease_Info_Message(buffer, &message);

                        // do stuff with message.

                        printf("%s %s %s\n", message.identity, message.lease_name, message.lease_namespace);
                        printf("Processed SET_LEASE_INFO_MSG_ID\n");
                    }
                    else{
                        printf("[Warning]: Client sent a message with an unhandled message type. Type: %i\n", message_header->message_type);
                        //client_connection->socket = 0;
                        //server.clients -= 1;
                        break;
                        // TODO: Disconnect client properly.
                    }

                    bytes_processed += message_header->message_length;
                }

                if(bytes_processed == client_connection->recv_buffered){
                    client_connection->recv_buffered = 0;
                } else{
                    int bytes_unprocessed = client_connection->recv_buffered - bytes_processed;
                    memcpy(client_connection->recv_buffer, client_connection->recv_buffer + bytes_processed, bytes_unprocessed);
                    client_connection->recv_buffered = bytes_unprocessed;
                }

                client = client->next;
            }
        }

        if(process_control.should_exit){
            close(db.server.socket);
            exit(0);
        }
        else sleep(1);
    }

    return 0;
}
