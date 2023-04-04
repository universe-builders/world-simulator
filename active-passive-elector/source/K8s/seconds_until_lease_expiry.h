#pragma once

typedef struct v1_lease_t v1_lease_t;

int seconds_until_lease_expiry(v1_lease_t* lease);