#pragma once

typedef struct Set_Lease_Info_Message Set_Lease_Info_Message;

/**
 * @return -1 on error, -2 on buffer too small to write message to, otherwise the size of the message buffered.
*/
int serialize_Set_Lease_Info_Message(void* buffer, int max_length, Set_Lease_Info_Message* message);