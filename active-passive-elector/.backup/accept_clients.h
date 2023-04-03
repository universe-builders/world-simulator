#pragma once

typedef struct TCP_Server TCP_Server;

/**
 * @return -1 for error, otherwise 0.
*/
int accept_clients(TCP_Server* server);
