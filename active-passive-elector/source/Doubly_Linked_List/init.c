#include <string.h> // memcpy

#include "Doubly_Linked_List_Node.h"

void init_Doubly_Linked_List_Node(Doubly_Linked_List_Node* to_init, void* data, Doubly_Linked_List_Node* prev, Doubly_Linked_List_Node* next){
    to_init->next = next;
    to_init->prev = prev;
    to_init->data = data;
}