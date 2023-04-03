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
#include "Doubly_Linked_List/Doubly_Linked_List_Node.h"
#include "Doubly_Linked_List/init.h"
#include "Doubly_Linked_List/remove.h"
#include "TCP_Connection/TCP_Connection.h"
#include "messages/Message_Header.h"
#include "messages/message_types.h"
#include "messages/Set_Lease_Info_Message/Set_Lease_Info_Message.h"
#include "messages/Set_Lease_Info_Message/deserialize.h"
#include "messages/Set_Role_Message/Set_Role_Message.h"
#include "messages/Set_Role_Message/deserialize.h"

typedef struct Program_Database{
    TCP_Server               server;       //
    Doubly_Linked_List_Node* client_roles; //  
} Program_Database;

void disconnect_client(Program_Database* db, Doubly_Linked_List_Node* client){
    remove_node_from_doubly_linked_list(client);
    db->server.clients -= 1;
}

int init(Program_Database* db){
    if(init_tcp_server(&db->server, 5000) == -1){
        printf("Failed to init TCP server.\n");
        return -1;
    }

    db->client_roles = 0x00;
}

int main(int argc, char *argv[]){
    init_process_control();

    Program_Database db;
    if(init(&db) != 0){
        printf("Failed to init program.\n");
        exit(1);
    }

    while(1){
        if(accept_clients(&db.server) == -1){
            printf("Error when accepting clients. Error: %i\n", errno);
        }

        while(1){
            int status = accept_client(&db.server);
            if(status == -1){
                printf("Error when accepting client. errno: %i\n", errno);
            }
            else if(status == 0){
                break; // no clients to accept.
            }
            else{
                /*
                    I don't like that this is mimicing the structure of the clients.
                    eg if TCP_Server changes it's data structure then this code also needs to change.

                    The correct solution, I think, is to store the roles when the role has been determined.
                    The storage should be a map or something else similar, using a client-id which the server
                    should provide.
                */


                Doubly_Linked_List_Node* client_role = (Doubly_Linked_List_Node*)( malloc(sizeof(Doubly_Linked_List_Node)) );
                init_Doubly_Linked_List_Node(client_role, 0x00, 0x00, 0x00);
                int* client_role_value = (&client_role->data); // using data location to store client role int.
                *client_role_value = 0;

                if(db.client_roles == 0x00){
                    db.client_roles = client_role;
                }
                else{
                    // Add new client_role to front of DLL for O(1) insertion.
                    client_role->next     = db.client_roles;
                    client_role->prev     = 0x00;
                    db.client_roles->prev = client_role;
                    db.client_roles       = client_role;
                }
            }
        }

        if(db.server.clients > 0){
            Doubly_Linked_List_Node* client = db.server.client_connections;
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
