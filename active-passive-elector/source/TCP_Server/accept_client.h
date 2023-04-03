#pragma once

typedef struct TCP_Server TCP_Server;
typedef struct TCP_Connection TCP_Connection;

/**
 * @return -1 for error, 0 for no connection accepted, 1 for a connection accepted.
*/
int accept_client(TCP_Server* server, TCP_Connection* accepted_client_connection);
