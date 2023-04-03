#pragma once

#include "../types/byte.h"

#define MAX_RECV_BUFFER 1024

typedef struct TCP_Connection{
    int  socket;
    byte recv_buffer[MAX_RECV_BUFFER];
    int  recv_buffered;
} TCP_Connection;
