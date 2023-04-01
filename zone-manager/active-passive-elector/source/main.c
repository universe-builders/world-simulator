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
#include "TCP_Server/buffer_data_from_clients.h"
#include "messages/Message_Header.h"
#include "messages/message_types.h"
#include "messages/Set_Lease_Info_Message/Set_Lease_Info_Message.h"
#include "messages/Set_Lease_Info_Message/deserialize.h"
#include "messages/Set_Role_Message/Set_Role_Message.h"
#include "messages/Set_Role_Message/deserialize.h"

int main(int argc, char *argv[]){
    init_process_control();
    
    TCP_Server server;
    if(init_tcp_server(&server, 5000) == -1){
        printf("Failed to init TCP server.\n");
    }

    int client_roles[MAX_CLIENTS];
    memset(client_roles, 0, sizeof(client_roles));

    while(1){
        if(accept_clients(&server) == -1){
            printf("Error when accepting clients. Error: %i\n", errno);
        }

        buffer_data_from_clients(&server);

        int clients_read = 0;
        for(int client_index = 0; clients_read < server.clients && client_index < MAX_CLIENTS; ++client_index){
            TCP_Connection* client_connection = server.client_connections + client_index;
            if(client_connection->socket == 0) continue;

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
                }/*
                else if(message_header->message_type == SET_ROLE_MSG_ID){
                    Set_Role_Message message;
                    deserialize_Set_Role_Message(buffer, &message);

                    printf("Role: %i\n", message.role);
                    printf("Processed SET_ROLE_MSG\n");
                }*/
                else{
                    printf("[Warning]: Client sent a message with an unhandled message type. Type: %i\n", message_header->message_type);
                    client_connection->socket = 0;
                    server.clients -= 1;
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
        }

        if(process_control.should_exit){
            close(server.socket);
            exit(0);
        }
        else sleep(1);
    }

    return 0;
}



/*
        int clients_read = 0;
        for(int client_index = 0; clients_read < server.clients && client_index < MAX_CLIENTS; ++client_index){
            
            
            
            //int client_socket = server.client_socket[client_index];
            //if(client_socket == 0) continue;

            // peek at data
            
            clients_read += 1;
        }
        */

        /*
        const char* message = "Hello, Net!\n";
        for(int client_index = 0; client_index < server.clients; ++client_index){
            int client_socket = server.client_socket[client_index];
            if(write(client_socket, message, strlen(message)) == -1){
                close(client_socket);
                server.client_socket[client_index] = 0;
                server.clients -= 1;
            }
        }
        */
        
        //close(client_socket);