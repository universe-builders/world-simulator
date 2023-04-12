#pragma once

/**
 * Sent from Server -> Client to indicate the role.
*/
typedef struct Set_Role_Message{
    int role; // See Role/Role.h
} Set_Role_Message;