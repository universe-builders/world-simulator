#pragma once

typedef struct v1_lease_t v1_lease_t;
typedef struct apiClient_t apiClient_t;

v1_lease_t* get_lease(apiClient_t* api_client, char* lease_name, char* lease_namespace);
