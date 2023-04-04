// C STD
#define _XOPEN_SOURCE 700 // strptime
#include <time.h> // time_t, strptime, mktime
#include <string.h> // memset

// K8s
#include <apiClient.h> // apiClient
#include <CoordinationV1API.h> // v1_lease_t

int seconds_until_lease_expiry(v1_lease_t* lease){
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    strptime(lease->spec->renew_time, "%Y-%m-%dT%H:%M:%SZ", &tm);
    time_t lease_renew_seconds = mktime(&tm);

    time_t current_time_seconds = time(NULL);

    return lease_renew_seconds - current_time_seconds;
}