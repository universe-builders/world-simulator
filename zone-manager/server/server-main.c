#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#include "model/tcp-server.h"

typedef int bool;
#define true 1
#define false 0

typedef struct Process_Control{
    bool should_exit;
} Process_Control;

Process_Control process_control;
void init_process_control(){
    process_control.should_exit = false;
}

void signal_handler(int signo){
    if(signo == SIGINT){
        process_control.should_exit = true;
    }
}

int main(int argc, char *argv[])
{
    init_process_control();
    signal(SIGINT, signal_handler);

    TCP_Server server;
    server.socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(server.socket != 0){
        printf("Failed to create socket. Error: %i\n", errno);
        exit(1);
    }

    memset(&server.address, 0, sizeof(server.address));
    server.address.sin_family      = AF_INET;
    server.address.sin_addr.s_addr = htonl(INADDR_ANY);
    server.address.sin_port        = htons(5000); 
    if(bind(server.socket, (struct sockaddr*)&server.address, sizeof(server.address)) != 0){
        printf("Failed to bind socket with address. Error: %i\n", errno);
        exit(1);
    }
    if(listen(server.socket, 10) != 0){
        printf("Failed to listen to socker. Error: %i\n", errno);
        exit(1);
    }

    while(1){
        int client_socket = accept(server.socket, NULL, NULL);
        if(client_socket == -1 && errno != EAGAIN && errno != EWOULDBLOCK){
            printf("Failed to accept client socket. Error: %i\n", errno);
        }

        const char* message = "Hello, Net!\n";
        write(client_socket, message, strlen(message)); 
        close(client_socket);

        if(process_control.should_exit){
            close(server.socket);
            exit(0);
        }
        else sleep(1);
    }
}