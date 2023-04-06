#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include <c-net/TCP_Connection/TCP_Connection.h>
#include <c-net/TCP_Connection/buffer_data_from_connection.h>
#include <c-core/Process_Control/Process_Control.h>
#include <c-core/Process_Control/init.h>

#include "model/tcp-client.h"

#include "../../active-passive-elector/source/messages/message_types.h"
#include "../../active-passive-elector/source/messages/Message_Header.h"
#include "../../active-passive-elector/source/messages/Set_Lease_Info_Message/Set_Lease_Info_Message.h"
#include "../../active-passive-elector/source/messages/Set_Lease_Info_Message/serialize.h"
#include "../../active-passive-elector/source/messages/Set_Role_Message/Set_Role_Message.h"
#include "../../active-passive-elector/source/messages/Set_Role_Message/deserialize.h"

// Configuration.
#define CLIENT_TIMEOUT_SECONDS 10

typedef struct Client{
    TCP_Connection connection;
    time_t         last_heartbeat_time;
} Client;
typedef struct Program_Database{
    Client client;
    const char* server_ip;
    const char* identity;
} Program_Database;

void process_messages_for_client(Client* client){
    int bytes_processed = 0;
    while(bytes_processed < client->connection.recv_buffered){
        char* buffer = client->connection.recv_buffer + bytes_processed;
        Message_Header* message_header = (Message_Header*)buffer;
        if(message_header->message_length > client->connection.recv_buffered) break;

        if(message_header->message_type == HEARTBEAT_MSG_ID){
            client->last_heartbeat_time = time(0x00);
        }
        else if(message_header->message_type == SET_ROLE_MSG_ID){
            Set_Role_Message message;
            deserialize_Set_Role_Message(buffer, &message);

            printf("Role changed to: %i\n", message.role);
        }
        else{
            printf("[Warning]: Server sent a message with an unhandled message type. Type: %i\n", message_header->message_type);
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

int init(Program_Database* db, char *argv[]){
    db->server_ip = argv[1];
    db->identity  = argv[2];

    db->client.connection.socket = socket(AF_INET, SOCK_STREAM, 0);
    if(db->client.connection.socket == -1){
        printf("Failed to create socket. Error: %i\n", errno);
        return -1;
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(5001); 

    if(inet_pton(AF_INET, db->server_ip, &server_address.sin_addr) != 1){
        printf("Failed to convert IP address from textual. Error: %i\n", errno);
        return -1;
    } 

    if(connect(db->client.connection.socket, (const struct sockaddr*)&server_address, sizeof(server_address)) != 0){
       printf("Failed to connect to server. Error: %i\n", errno);
       return -1;
    }

    db->client.last_heartbeat_time = time(0x00);

    if(fcntl(db->client.connection.socket, F_SETFL, O_NONBLOCK) == -1){
        printf("Failed to set socket to non-blocking.\n");
        return -1;
    }

    Set_Lease_Info_Message set_lease_info_message;
    set_lease_info_message.identity = db->identity;
    set_lease_info_message.lease_name = "active-passive-elector-test-lease";
    set_lease_info_message.lease_namespace = "dev-env";

    char buffer[1024];
    int buffered = serialize_Set_Lease_Info_Message(&buffer, sizeof(buffer), &set_lease_info_message);
    if(write(db->client.connection.socket, &buffer, buffered) == -1){
        printf("Write failed. errno: %i\n", errno);
        return -1;
    }

    return 0;
}

void sigpipe_signal_handler(int signo){
    if(signo == SIGPIPE){
        // Do nothing, SIGPIPE means the connection closed on the server side.
        // This will be detected in the write or in heartbeat.
    }
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("\n Usage: %s <ip of server> <identity> \n", argv[0]);
        return 1;
    }

    signal(SIGPIPE, sigpipe_signal_handler);
    init_process_control();
    Program_Database* db = malloc(sizeof(Program_Database));
    if(init(db, argv) != 0){
        printf("Failed to init program db.\n");
        exit(1);
    }

    while(1){
        Message_Header heartbeat;
        heartbeat.message_length = sizeof(heartbeat);
        heartbeat.message_type   = HEARTBEAT_MSG_ID;
        if(write(db->client.connection.socket, (void*)(&heartbeat), sizeof(heartbeat)) == -1){
            if(errno == EPIPE){
                printf("[Critical]: Exiting due to connection to server was disrupted.\n");
                exit(1);
            }
            printf("Write failed. errno: %i\n", errno);
        }

        buffer_data_from_connection(&db->client.connection);
        process_messages_for_client(&db->client);

        if(time(0x00) - db->client.last_heartbeat_time > CLIENT_TIMEOUT_SECONDS){
            printf("[Critical]: Exiting due to not receiving heartbeat within timeout.\n");
            exit(1);
        }

        if(process_control.should_exit){
            shutdown(db->client.connection.socket, SHUT_RDWR);
            close(db->client.connection.socket);
            exit(0);
        }

        sleep(1);
    }

    return 0;
}