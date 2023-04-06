#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "Client.h"
#include "../messages/Message_Header.h"
#include "../messages/message_types.h"
#include "../messages/Set_Lease_Info_Message/Set_Lease_Info_Message.h"
#include "../messages/Set_Lease_Info_Message/deserialize.h"

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
