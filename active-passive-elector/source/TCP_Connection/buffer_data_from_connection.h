#pragma once

typedef struct TCP_Connection TCP_Connection;

/**
 * @return -1 for error, otherwise the number of bytes buffered.
*/
int buffer_data_from_connection(TCP_Connection* connection);