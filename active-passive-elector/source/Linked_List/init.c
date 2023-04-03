#include <string.h> // memcpy

#include "Linked_List_Node.h"

void init_Linked_List_Node(Linked_List_Node* to_init, void* data){
    to_init->next = 0x00;
    to_init->data = data;
}