#pragma once

typedef struct TCP_Server TCP_Server;

/**
 * @return -1 for error, 0 for no connection accepted, 1 for a connection accepted.
*/
int accept_client(TCP_Server* server);
