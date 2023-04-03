#pragma once

typedef struct TCP_Server TCP_Server;
typedef struct TCP_Connection TCP_Connection;

void buffer_data_from_client(TCP_Server* server, TCP_Connection* client);