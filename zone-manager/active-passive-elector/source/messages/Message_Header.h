#pragma once

typedef struct Message_Header{
    int message_length; // Includes length of header.
    int message_type;   // See message-types.h
} Message_Header;