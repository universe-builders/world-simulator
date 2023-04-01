#ifndef renew_active_lease_h
#define renew_active_lease_h

#include "../role/role.h"
typedef struct apiClient_t apiClient_t;

Role renew_active_lease(apiClient_t* apiClient);

#endif