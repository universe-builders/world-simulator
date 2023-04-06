#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <kubernetes/include/apiClient.h>
#include <kubernetes/api/CoordinationV1API.h>

#include <c-core/Process_Control/Process_Control.h>
#include <c-core/Process_Control/init.h>
#include <c-net/TCP_Server/TCP_Server.h>
#include <c-net/TCP_Server/init.h>
#include <c-net/TCP_Server/accept_client.h>
#include <c-net/TCP_Connection/TCP_Connection.h>
#include <c-net/TCP_Connection/buffer_data_from_connection.h>
#include <c-ds/Linked_List/Linked_List_Node.h>
#include <c-ds/Doubly_Linked_List/Doubly_Linked_List_Node.h>
#include <c-ds/Doubly_Linked_List/init.h>
#include <c-ds/Doubly_Linked_List/remove.h>
#include <c-k8s/api_client/initialize_k8s_core_api_client.h>
#include <c-k8s/lease/get_lease.h>
#include <c-k8s/lease/seconds_until_lease_expiry.h>
#include <c-k8s/lease/calculate_lease_next_renew_time.h>

#include "Role/Role.h"
#include "messages/Message_Header.h"
#include "messages/message_types.h"
#include "messages/Set_Lease_Info_Message/Set_Lease_Info_Message.h"
#include "messages/Set_Lease_Info_Message/deserialize.h"
#include "messages/Set_Role_Message/Set_Role_Message.h"
#include "messages/Set_Role_Message/serialize.h"

// Configuration.
#define CLIENT_TIMEOUT_SECONDS 10
#define SECONDS_BEFORE_EXPIRY_BEFORE_RENEWING 2
#define MAX_IDENTITY_STRING_LEN 255
#define MAX_LEASE_NAME_STRING_LEN 255
#define MAX_LEASE_NAMESPACE_STRING_LEN 255

typedef struct Client{
    TCP_Connection connection;
    int            role;
    time_t         last_heartbeat_time;
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
    if(init_tcp_server(&db->server, 5001) == -1){
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

/**
 * @return -1 for error, otherwise 0.
*/
int process_messages_for_client(Client* client){
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
                return -1;
            }
            const int lease_name_str_len = strlen(message.lease_name);
            if(lease_name_str_len > MAX_LEASE_NAME_STRING_LEN){
                printf("[Warning]: Client lease name exceeded maximum length of %i. Identity: %s.", MAX_LEASE_NAME_STRING_LEN, message.lease_name);
                return -1;
            }
            const int lease_namespace_str_len = strlen(message.lease_namespace);
            if(lease_namespace_str_len > MAX_LEASE_NAMESPACE_STRING_LEN){
                printf("[Warning]: Client lease namespace exceeded maximum length of %i. Identity: %s.", MAX_LEASE_NAMESPACE_STRING_LEN, message.lease_namespace);
                return -1;
            }

            // Store data in client.
            memcpy(client->identity, message.identity, identity_str_len + 1);
            memcpy(client->lease_name, message.lease_name, lease_name_str_len + 1);
            memcpy(client->lease_namespace, message.lease_namespace, lease_namespace_str_len + 1);

            printf("%s %s %s\n", client->identity, client->lease_name, client->lease_namespace);
            printf("Processed SET_LEASE_INFO_MSG_ID\n");
        }
        else if(message_header->message_type == HEARTBEAT_MSG_ID){
            client->last_heartbeat_time = time(0x00);

            // Return heartbeat.
            Message_Header heartbeat;
            heartbeat.message_length = sizeof(heartbeat);
            heartbeat.message_type   = HEARTBEAT_MSG_ID;
            if(write(client->connection.socket, (void*)(&heartbeat), sizeof(heartbeat)) == -1){
                printf("Write failed. errno: %i\n", errno);
                return -1;
            }
        }
        else{
            printf("[Warning]: Client sent a message with an unhandled message type. Type: %i\n", message_header->message_type);
            return -1;
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

    return 0;
}

void attempt_to_update_role(Program_Database* db, Client* client){
    // Attempt to change role, if appropriate.
    int role_before_changes = client->role;
    v1_lease_t* lease = get_lease(db->k8s_api_client, client->lease_name, client->lease_namespace);
    if(lease == 0x00 || db->k8s_api_client->response_code != 200){
        printf("[Warning]: Failed to get lease. Lease Name: %s. Namespace: %s.\n", client->lease_name, client->lease_namespace);
        v1_lease_free(lease);
    }
    // Lease expired or unowned, try to grab it and become active otherwise become passive.
    else if(lease->spec->holder_identity == 0x00 || lease->spec->renew_time == 0x00 || seconds_until_lease_expiry(lease) <= 0){
        lease->spec->holder_identity = client->identity;

        char next_renew_time[256];
        calculate_lease_next_renew_time(next_renew_time, sizeof(next_renew_time), lease);
        lease->spec->renew_time = next_renew_time;

        v1_lease_t* replaced_lease = CoordinationV1API_replaceNamespacedLease(
            db->k8s_api_client,
            client->lease_name,
            client->lease_namespace, 
            lease, 
            0x00, 0x00, 0x00, 0x00
        );
        // Replaced lease has client as identity, replacement was successful.
        if(db->k8s_api_client->response_code == 200 && strcmp(replaced_lease->spec->holder_identity, client->identity) == 0){
            client->role = ROLE_ACTIVE;
        }
        // Replacement was unsuccessful, assume request was unsuccessful due to race condition
        // and another client become active.
        else{
            client->role = ROLE_PASSIVE;
        }

        v1_lease_free(replaced_lease);
    }
    // Lease is owned by this identity, become active and renew if needed.
    else if(lease->spec->holder_identity != 0x00 && strcmp(lease->spec->holder_identity, client->identity) == 0){
        client->role = ROLE_ACTIVE;
        
        // Lease is ready to be renewed.
        if(lease->spec->renew_time == 0x00 || seconds_until_lease_expiry(lease) <= SECONDS_BEFORE_EXPIRY_BEFORE_RENEWING){
            char next_renew_time[256];
            calculate_lease_next_renew_time(next_renew_time, sizeof(next_renew_time), lease);
            lease->spec->renew_time = next_renew_time;
            v1_lease_t* replaced_lease = CoordinationV1API_replaceNamespacedLease(
                db->k8s_api_client,
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
        else{
            v1_lease_free(lease);
        }
    }
    // Lease is owned, become passive.
    else{
        client->role = ROLE_PASSIVE;
    }

    // Role changed, update client.
    if(client->role != role_before_changes){
        Set_Role_Message set_role_message;
        set_role_message.role = client->role;
        char buffer[256];
        int buffered = serialize_Set_Role_Message(buffer, sizeof(buffer), &set_role_message);
        write(client->connection.socket, &buffer, buffered);
    }
}

int main(int argc, char *argv[]){
    init_process_control();

    Program_Database* db = (Program_Database*)( malloc(sizeof(Program_Database)) );
    if(init(db) != 0){
        printf("Failed to init program.\n");
        exit(1);
    }

    while(1){
        // Accept new clients.
        while(accept_client(&db->server, &db->next_client->connection) == 1){
            db->next_client->last_heartbeat_time = time(0x00);

            // Insert new client at start of list.            
            Doubly_Linked_List_Node* client_node = malloc(sizeof(Doubly_Linked_List_Node));
            client_node->data = db->next_client;
            client_node->prev = 0x00;
            client_node->next = db->clients;
            if(db->clients != 0x00){ db->clients->prev = client_node; }
            db->clients = client_node;
            db->number_of_clients += 1;

            init_next_client(db);
        }

        if(db->number_of_clients > 0){
            Doubly_Linked_List_Node* client_node = db->clients;
            while(client_node != 0x00){
                Client* client = (Client*)( client_node->data );

                #define disconnect_client() \
                Doubly_Linked_List_Node* client_node_to_dc = client_node; \
                client_node = client_node->next; \
                shutdown(client->connection.socket, SHUT_RDWR); \
                close(client->connection.socket); \
                if(client_node_to_dc->prev != 0x00){ \
                    client_node_to_dc->prev->next = client_node_to_dc->next; \
                } \
                if(client_node_to_dc->next != 0x00){ \
                    client_node_to_dc->next->prev = client_node_to_dc->prev; \
                } \
                if(db->clients == client_node_to_dc){ \
                    db->clients = client_node_to_dc->next; \
                } \
                db->number_of_clients -= 1; \
                free(client); \
                free(client_node_to_dc);

                if(buffer_data_from_connection(&client->connection) == -1){
                    printf("[Info] Disconnecting client due to error in buffering data from conn.\n");
                    disconnect_client();
                    continue;
                }
                if(process_messages_for_client(client) == -1){
                    printf("[Info] Disconnecting client due to error in processing messages from conn.\n");
                    disconnect_client();
                    continue;
                }

                // Disconnect client it it has failed to heartbeat within timeout.
                if(time(0x00) - client->last_heartbeat_time > CLIENT_TIMEOUT_SECONDS){
                    printf("[Info] Disconnected client due to heartbeat timeout.\n");
                    disconnect_client();
                    continue;
                }

                // Skip trying to change role if no identity or lease provided by client yet.
                if(client->identity[0] == 0 || client->lease_name[0] == 0 || client->lease_namespace[0] == 0){
                    client_node = client_node->next;
                    continue;
                }

                // Attempt to update role, if appropriate.
                attempt_to_update_role(db, client);

                client_node = client_node->next;
            }
        }

        if(process_control.should_exit){
            close(db->server.socket);
            exit(0);
        }
        else sleep(1);
    }

    return 0;
}
