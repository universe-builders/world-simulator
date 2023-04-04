// C STD
#define _XOPEN_SOURCE 700 // strptime
#include <time.h> // time_t, strptime, mktime
#include <string.h> // memset

// K8s
#include <apiClient.h> // apiClient
#include <CoordinationV1API.h> // v1_lease_t

void calculate_lease_next_renew_time(char* buffer, int size_of_buffer, v1_lease_t* lease){
    time_t time_epoch = time(0x00);
    time_t seconds_since_epoch_renew_time = time_epoch + lease->spec->lease_duration_seconds;

    // epoch -> tm
    struct tm* tm_after_change = gmtime(&seconds_since_epoch_renew_time);

    // tm -> ISO 8601. .999999 added as API expects microseconds but strftime doesn't provide microseconds.
    strftime(buffer, size_of_buffer, "%Y-%m-%dT%H:%M:%S.999999Z", tm_after_change);
}