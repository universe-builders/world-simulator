#include <string.h> // memcpy

#include "Linked_List_Node.h"

void init_Linked_List_Node(Linked_List_Node* to_init, void* data, Linked_List_Node* next){
    to_init->next = next;
    to_init->data = data;
}