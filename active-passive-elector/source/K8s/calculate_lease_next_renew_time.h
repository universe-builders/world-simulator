#pragma once

typedef struct v1_lease_t v1_lease_t;

void calculate_lease_next_renew_time(char* buffer, int size_of_buffer, v1_lease_t* lease);