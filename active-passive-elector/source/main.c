// C STD
#include <errno.h> // errno
#include <stdio.h> // printf
#include <stdlib.h>
#include <unistd.h>
#include <string.h> // memcpy
#include <sys/types.h>

// Linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// K8s
#include <apiClient.h> // apiClient
#include <CoordinationV1API.h> // v1_lease_t

// Universe Builders
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
#include "K8s/initialize_k8s_core_api_client.h"
#include "K8s/get_lease.h"
#include "K8s/seconds_until_lease_expiry.h"
#include "K8s/calculate_lease_next_renew_time.h"

#define MAX_IDENTITY_STRING_LEN 255
#define MAX_LEASE_NAME_STRING_LEN 255
#define MAX_LEASE_NAMESPACE_STRING_LEN 255
typedef struct Client{
    TCP_Connection connection;
    int            role;
    char           identity[MAX_IDENTITY_STRING_LEN + 1];
    char           lease_name[MAX_LEASE_NAME_STRING_LEN + 1];
    char           lease_namespace[MAX_LEASE_NAMESPACE_STRING_LEN + 1];
} Client;

typedef struct Program_Database{
    TCP_Server               server;
    Doubly_Linked_List_Node* clients;
    int                      number_of_clients;
    Client*                  next_client;
    apiClient_t*             k8s_api_client;
} Program_Database;

void init_next_client(Program_Database* db){
    db->next_client = malloc(sizeof(Client));
    db->next_client->role = ROLE_UNKNOWN;
    db->next_client->identity[0] = 0;
    db->next_client->lease_name[0] = 0;
    db->next_client->lease_namespace[0] = 0;
}

int init(Program_Database* db){
    if(init_tcp_server(&db->server, 5000) == -1){
        printf("Failed to init TCP server.\n");
        return -1;
    }

    db->clients           = 0x00;
    db->number_of_clients = 0;

    init_next_client(db);

    db->k8s_api_client = initialize_k8s_core_api_client();
    if(db->k8s_api_client == 0x00){
        printf("Failed to initialize K8s API Client\n");
        return -1;
    }

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

            // Validate data.
            const int identity_str_len = strlen(message.identity);
            if(identity_str_len > MAX_IDENTITY_STRING_LEN){
                printf("[Warning]: Client identity exceeded maximum length of %i. Identity: %s.", MAX_IDENTITY_STRING_LEN, message.identity);
            }
            const int lease_name_str_len = strlen(message.lease_name);
            if(lease_name_str_len > MAX_LEASE_NAME_STRING_LEN){
                printf("[Warning]: Client lease name exceeded maximum length of %i. Identity: %s.", MAX_LEASE_NAME_STRING_LEN, message.lease_name);
            }
            const int lease_namespace_str_len = strlen(message.lease_namespace);
            if(lease_namespace_str_len > MAX_LEASE_NAMESPACE_STRING_LEN){
                printf("[Warning]: Client lease namespace exceeded maximum length of %i. Identity: %s.", MAX_LEASE_NAMESPACE_STRING_LEN, message.lease_namespace);
            }

            // Store data in client.
            memcpy(client->identity, message.identity, identity_str_len + 1);
            memcpy(client->lease_name, message.lease_name, lease_name_str_len + 1);
            memcpy(client->lease_namespace, message.lease_namespace, lease_namespace_str_len + 1);

            printf("%s %s %s\n", client->identity, client->lease_name, client->lease_namespace);
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

                // Skip trying to adjust role if no identity or lease provided by client yet.
                if(client->identity[0] == 0 || client->lease_name[0] == 0 || client->lease_namespace[0] == 0){
                    client_node = client_node->next;
                    continue;
                }

                v1_lease_t* lease = get_lease(db.k8s_api_client, client->lease_name, client->lease_namespace);
                if(lease == 0x00){
                    printf("[Warning]: Failed to get lease. Lease Name: %s. Namespace: %s.\n", client->lease_name, client->lease_namespace);
                }
                else if(client->role == ROLE_UNKNOWN){
                    // Owned by this, make active.
                    if(lease->spec->holder_identity != 0x00 && strcmp(lease->spec->holder_identity, client->identity) == 0){
                        client->role = ROLE_ACTIVE;
                    }
                    // Not owned by this, make passive.
                    else{
                        client->role = ROLE_PASSIVE;
                    }

                    v1_lease_free(lease);
                }
                else if(client->role == ROLE_PASSIVE){
                    // Lease is owned by this, make active.
                    if(lease->spec->holder_identity != 0x00 && strcmp(lease->spec->holder_identity, client->identity) == 0){
                        client->role = ROLE_ACTIVE;

                        v1_lease_free(lease);
                    }
                    // Lease has expired or has no owner, try to grab it.
                    else if(lease->spec->holder_identity == 0x00 || lease->spec->renew_time == 0x00 || seconds_until_lease_expiry(lease) <= 0){
                        lease->spec->holder_identity = client->identity;

                        char next_renew_time[256];
                        calculate_lease_next_renew_time(next_renew_time, sizeof(next_renew_time), lease);
                        lease->spec->renew_time = next_renew_time;

                        v1_lease_t* replaced_lease = CoordinationV1API_replaceNamespacedLease(
                            db.k8s_api_client,
                            client->lease_name,
                            client->lease_namespace, 
                            lease, 
                            0x00, 0x00, 0x00, 0x00
                        );
                        if(replaced_lease == 0x00){
                            printf("[Warning]: Failed to replace lease. Lease Name: %s. Namespace: %s.\n", client->lease_name, client->lease_namespace);
                        }
                        // Replaced lease has client as identity, replacement was successful.
                        else if(db.k8s_api_client->response_code == 200 && strcmp(replaced_lease->spec->holder_identity, client->identity) == 0){
                            client->role = ROLE_ACTIVE;
                        }

                        v1_lease_free(replaced_lease);
                    }                    
                }
                else if(client->role == ROLE_ACTIVE){
                    // Lease is not owned by this client, become passive.
                    if(strcmp(lease->spec->holder_identity, client->identity) != 0){
                        client->role = ROLE_PASSIVE;

                        v1_lease_free(lease);
                    }
                    else if(lease->spec->renew_time == 0x00 || seconds_until_lease_expiry(lease) <= 2){
                        char next_renew_time[256];
                        calculate_lease_next_renew_time(next_renew_time, sizeof(next_renew_time), lease);
                        lease->spec->renew_time = next_renew_time;
                        v1_lease_t* replaced_lease = CoordinationV1API_replaceNamespacedLease(
                            db.k8s_api_client,
                            client->lease_name,
                            client->lease_namespace, 
                            lease, 
                            0x00, 0x00, 0x00, 0x00
                        );
                        if(replaced_lease == 0x00){
                            printf("[Warning]: Failed to replace lease. Lease Name: %s. Namespace: %s.\n", client->lease_name, client->lease_namespace);
                        }

                        v1_lease_free(replaced_lease);
                    }
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
