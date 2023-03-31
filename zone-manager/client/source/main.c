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

#include "model/tcp-client.h"

#include "../../active-passive-elector/source/messages/message_types.h"
#include "../../active-passive-elector/source/messages/Message_Header.h"
#include "../../active-passive-elector/source/messages/Set_Lease_Info_Message/Set_Lease_Info_Message.h"
#include "../../active-passive-elector/source/messages/Set_Lease_Info_Message/serialize.h"
#include "../../active-passive-elector/source/messages/Set_Role_Message/Set_Role_Message.h"
#include "../../active-passive-elector/source/messages/Set_Role_Message/serialize.h"

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

    char recvBuff[1024];
    memset(recvBuff, 0, sizeof(recvBuff));

    TCP_Client client;
    client.socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client.socket == -1){
        printf("Failed to create socket. Error: %i\n", errno);
        exit(1);
    }
    memset(&client.server_address, 0, sizeof(client.server_address));
    client.server_address.sin_family = AF_INET;
    client.server_address.sin_port   = htons(5000); 

    if(inet_pton(AF_INET, argv[1], &client.server_address.sin_addr) != 1){
        printf("Failed to convert IP address from textual. Error: %i\n", errno);
        exit(1);
    } 

    if(connect(client.socket, (const struct sockaddr*)&client.server_address, sizeof(client.server_address)) != 0){
       printf("Failed to connect to server. Error: %i\n", errno);
       exit(1);
    }

/*
    Set_Lease_Info_Message set_lease_info_message;
    set_lease_info_message.identity = "my-identity-is-this";
    set_lease_info_message.lease_name = "some-lease-name i guess";
    set_lease_info_message.lease_namespace = "namespaces would be cool in C";

    char buffer[1024];
    int buffered = serialize_Set_Lease_Info_Message(&buffer, sizeof(buffer), &set_lease_info_message);
*/

    Set_Role_Message set_role_message;
    set_role_message.role = 12999;
    char buffer[1024];
    int buffered = serialize_Set_Role_Message(&buffer, sizeof(buffer), &set_role_message);

    if(write(client.socket, &buffer, buffered) == -1){
        printf("Write failed. errno: %i\n", errno);
    }


    /*

    while(1){
        int bytes_read = read(client.socket, recvBuff, sizeof(recvBuff) - 1);
        if(fputs(recvBuff, stdout) == EOF){
            printf("Failed to write to stdout. Errno: %i\n", errno);
            exit(1);
        }
        exit(0);
    }
    */

    return 0;
}