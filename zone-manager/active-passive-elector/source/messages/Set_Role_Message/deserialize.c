#include "Set_Role_Message.h"
#include "../message_types.h"
#include "../Message_Header.h"

void deserialize_Set_Role_Message(void* buffer, Set_Role_Message* deserialized){
    Message_Header* message_header = (Message_Header*)buffer;
    int* role = (int*)(buffer + sizeof(Message_Header));
    deserialized->role = *role;
}