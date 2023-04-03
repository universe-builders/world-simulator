# pragma once

typedef struct Doubly_Linked_List_Node{
    struct Doubly_Linked_List_Node* next;
    struct Doubly_Linked_List_Node* prev;
    
    void* data;
} Doubly_Linked_List_Node;