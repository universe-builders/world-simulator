#pragma once

typedef struct Set_Role_Message Set_Role_Message;

/**
 * @return -1 on error, -2 on buffer too small to write message to, otherwise the size of the message buffered.
*/
int serialize_Set_Role_Message(void* buffer, int max_length, Set_Role_Message* message);