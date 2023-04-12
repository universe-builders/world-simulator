#pragma once

//#include <c-net/TCP_Server/TCP_Server.h>

//typedef struct Doubly_Linked_List_Node Doubly_Linked_List_Node;
//typedef struct Client Client;
typedef struct apiClient_t apiClient_t;

typedef struct Program_Database{
    //TCP_Server               server;
    //Doubly_Linked_List_Node* clients;
    //int                      number_of_clients;
    //Client*                  next_client;
    apiClient_t*             k8s_api_client;
} Program_Database;