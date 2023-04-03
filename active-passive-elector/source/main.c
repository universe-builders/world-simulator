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
#include "TCP_Server/accept_client.h"
#include "Linked_List/Linked_List_Node.h"
#include "Doubly_Linked_List/Doubly_Linked_List_Node.h"
#include "Doubly_Linked_List/init.h"
#include "Doubly_Linked_List/remove.h"
#include "TCP_Connection/TCP_Connection.h"
#include "TCP_Connection/buffer_data_from_connection.h"
#include "Role/Role.h"
#include "messages/Message_Header.h"
#include "messages/message_types.h"
#include "messages/Set_Lease_Info_Message/Set_Lease_Info_Message.h"
#include "messages/Set_Lease_Info_Message/deserialize.h"
#include "messages/Set_Role_Message/Set_Role_Message.h"
#include "messages/Set_Role_Message/deserialize.h"

typedef struct Client{
    TCP_Connection connection;
    int            role;
} Client;

typedef struct Program_Database{
    TCP_Server               server;
    Doubly_Linked_List_Node* clients;
    int                      number_of_clients;
    Client*                  next_client;
} Program_Database;

void init_next_client(Program_Database* db){
    db->next_client = malloc(sizeof(Client));
    db->next_client->role = ROLE_UNKNOWN;
}

int init(Program_Database* db){
    if(init_tcp_server(&db->server, 5000) == -1){
        printf("Failed to init TCP server.\n");
        return -1;
    }

    db->clients           = 0x00;
    db->number_of_clients = 0;

    init_next_client(db);

    return 0;
}

void process_messages_for_client(Client* client){
    int bytes_processed = 0;
    while(bytes_processed < client->connection.recv_buffered){
        char* buffer = client->connection.recv_buffer + bytes_processed;
        Message_Header* message_header = (Message_Header*)buffer;
        if(message_header->message_length > client->connection.recv_buffered) break;
    
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

    if(bytes_processed == client->connection.recv_buffered){
        client->connection.recv_buffered = 0;
    } else{
        int bytes_unprocessed = client->connection.recv_buffered - bytes_processed;
        memcpy(client->connection.recv_buffer, client->connection.recv_buffer + bytes_processed, bytes_unprocessed);
        client->connection.recv_buffered = bytes_unprocessed;
    }
}

int main(int argc, char *argv[]){
    init_process_control();

    Program_Database db;
    if(init(&db) != 0){
        printf("Failed to init program.\n");
        exit(1);
    }

    while(1){
        // Accept new clients.
        while(accept_client(&db.server, &db.next_client->connection) == 1){
            // Insert new client at start of list.            
            Doubly_Linked_List_Node* client_node = malloc(sizeof(Doubly_Linked_List_Node));
            client_node->data = db.next_client;
            client_node->prev = 0x00;
            client_node->next = db.clients;
            if(db.clients != 0x00){ db.clients->prev = client_node; }
            db.clients = client_node;
            db.number_of_clients += 1;

            init_next_client(&db);
        }

        if(db.number_of_clients > 0){
            Doubly_Linked_List_Node* client_node = db.clients;
            while(client_node != 0x00){
                Client* client = (Client*)( client_node->data );

                buffer_data_from_connection(&client->connection);
                process_messages_for_client(client);
                
                if(client->role == ROLE_UNKNOWN){

                    

                    // get lease
                    // if lease current
                        // if owned by this
                            // make active
                        // if not owned by this
                            // make passive
                    // if lease expired
                        // attempt grab
                        // if grabbed
                            // make active
                        // if not grabbed
                            // stay unknown
                } 
                else if(client->role == ROLE_PASSIVE){
                    // get lease
                    // if lease current
                        // if owned by this
                            // make active
                        // if not owned by this
                            // stay passive
                    // if lease expired
                        // grab
                        // if grabbed
                            // make active
                        // if not grabbed
                            // stay passive
                } 
                else if(client->role == ROLE_ACTIVE){
                    // get lease

                    // if not owned by this
                        // make passive
                    // else 
                        // if lease expiration within 1s
                            // update lease
                }

                client_node = client_node->next;
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
