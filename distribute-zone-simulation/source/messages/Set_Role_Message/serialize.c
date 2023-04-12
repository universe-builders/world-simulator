#include <string.h> // strlen, memcpy
#include "../message_types.h"
#include "../Message_Header.h"
#include "Set_Role_Message.h"

int serialize_Set_Role_Message(void* buffer, int max_length, Set_Role_Message* message){
    int message_length = sizeof(Message_Header) + sizeof(message->role);
    
    if(max_length < message_length) return -2;
    
    Message_Header* message_header = (Message_Header*)buffer;
    message_header->message_length = message_length;
    message_header->message_type = SET_ROLE_MSG_ID;

    memcpy(buffer + sizeof(Message_Header), (void*)(&message->role), sizeof(message->role));

    return message_length;
}