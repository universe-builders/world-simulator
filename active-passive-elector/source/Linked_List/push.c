#include "Linked_List_Node.h"

void push_node_on_linked_list(Linked_List_Node* head, Linked_List_Node* to_push){
    Linked_List_Node* end_of_list = head;
    while(end_of_list->next != 0x00){ end_of_list = end_of_list->next; }

    end_of_list->next = to_push;
}