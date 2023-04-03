#include "Doubly_Linked_List_Node.h"

void remove_node_from_doubly_linked_list(Doubly_Linked_List_Node* node_to_remove){
    node_to_remove->prev->next = node_to_remove->next;
    if(node_to_remove->next != 0x00){
        node_to_remove->next->prev = node_to_remove->prev;
    }
}