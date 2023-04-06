#pragma once

typedef struct Client Client;

/**
 * @return -1 for error, otherwise 0.
*/
int process_messages_for_client(Client* client);