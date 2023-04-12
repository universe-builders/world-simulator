#pragma once

#include <time.h>
#include <c-net/TCP_Connection/TCP_Connection.h>


//#include <c-net/TCP_Server/TCP_Server.h>

//typedef struct Doubly_Linked_List_Node Doubly_Linked_List_Node;
//typedef struct Client Client;
typedef struct apiClient_t apiClient_t;

typedef struct Active_Passive_Elector_Client{
    TCP_Connection connection;
    time_t         last_heartbeat_time;
} Active_Passive_Elector_Client;

typedef struct Program_Database{
    // K8s
    apiClient_t* k8s_api_client;

    //TCP_Server               server;
    //Doubly_Linked_List_Node* clients;
    //int                      number_of_clients;
    //Client*                  next_client;

    // Active Passive Elector
    Active_Passive_Elector_Client active_passive_elector_client;
    
} Program_Database;