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

    while(1){
        int bytes_read = read(client.socket, recvBuff, sizeof(recvBuff) - 1);
        if(fputs(recvBuff, stdout) == EOF){
            printf("Failed to write to stdout. Errno: %i\n", errno);
            exit(1);
        }
        exit(0);
    }

    return 0;
}