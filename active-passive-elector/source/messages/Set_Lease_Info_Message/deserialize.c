#include <string.h> // strlen
#include "Set_Lease_Info_Message.h"
#include "../message_types.h"
#include "../Message_Header.h"

void deserialize_Set_Lease_Info_Message(void* buffer, Set_Lease_Info_Message* deserialized){
    Message_Header* message_header = (Message_Header*)buffer;
    int message_bytes_processed = sizeof(Message_Header);

    int str_len = strlen(buffer + message_bytes_processed) + 1;
    deserialized->identity = (char*)(buffer + message_bytes_processed);
    message_bytes_processed += str_len;

    str_len = strlen(buffer + message_bytes_processed) + 1;
    deserialized->lease_name = (char*)(buffer + message_bytes_processed);
    message_bytes_processed += str_len;

    str_len = strlen(buffer + message_bytes_processed) + 1;
    deserialized->lease_namespace = (char*)(buffer + message_bytes_processed);
    message_bytes_processed += str_len;
}