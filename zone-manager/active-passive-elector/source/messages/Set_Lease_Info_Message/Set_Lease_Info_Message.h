#pragma once

/**
 * Sent from Client -> Server to set the lease information for the connection.
*/
typedef struct Set_Lease_Info_Message{
    const char* identity;
    const char* lease_name;
    const char* lease_namespace;
} Set_Lease_Info_Message;