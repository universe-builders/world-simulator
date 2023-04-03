#pragma once

typedef struct TCP_Server TCP_Server;
typedef struct TCP_Connection TCP_Connection;

void disconnect_client(TCP_Server* server, TCP_Connection* client_to_disconnect);