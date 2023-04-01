#include <string.h> // strlen, memcpy
#include "../message_types.h"
#include "../Message_Header.h"
#include "Set_Lease_Info_Message.h"

int serialize_Set_Lease_Info_Message(void* buffer, int max_length, Set_Lease_Info_Message* message){
    int copied = sizeof(Message_Header); // leave room for header at start of buffer.

    int length_to_copy = 0;
    
    length_to_copy = strlen(message->identity) + 1;
    if(copied + length_to_copy > max_length) return -2;
    memcpy(buffer + copied, message->identity, length_to_copy);
    copied += length_to_copy;

    length_to_copy = strlen(message->lease_name) + 1;
    if(copied + length_to_copy > max_length) return -2;
    memcpy(buffer + copied, message->lease_name, length_to_copy);
    copied += length_to_copy;

    length_to_copy = strlen(message->lease_namespace) + 1;
    if(copied + length_to_copy > max_length) return -2;
    memcpy(buffer + copied, message->lease_namespace, length_to_copy);
    copied += length_to_copy;

    Message_Header* message_header = (Message_Header*)buffer;
    message_header->message_length = copied;
    message_header->message_type = SET_LEASE_INFO_MSG_ID;

    return copied;
}