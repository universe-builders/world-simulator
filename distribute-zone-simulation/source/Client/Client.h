#pragma once

#include <c-net/TCP_Connection/TCP_Connection.h>
#include <time.h>

#include "../configuration.h"

typedef struct Client{
    TCP_Connection connection;
    int            role;
    time_t         last_heartbeat_time;
    char           identity[MAX_IDENTITY_STRING_LEN + 1];
    char           lease_name[MAX_LEASE_NAME_STRING_LEN + 1];
    char           lease_namespace[MAX_LEASE_NAMESPACE_STRING_LEN + 1];
} Client;