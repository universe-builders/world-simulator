#include <stdlib.h>

#include "Client.h"
#include "../Program_Database/Program_Database.h"
#include "../Role.h"

void init_next_client(Program_Database* db){
    db->next_client = malloc(sizeof(Client));
    db->next_client->role = ROLE_UNKNOWN;
    db->next_client->identity[0] = 0;
    db->next_client->lease_name[0] = 0;
    db->next_client->lease_namespace[0] = 0;
}