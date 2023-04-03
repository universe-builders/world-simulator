# Responsibilities
* Provides a linked list data structure.

# Usage
* Can use an external data pointer, to structure data that already exists.
* Can use internal data, ie instead of use node->data can emplace the data and then read it via (Type*)(&node->data), and when allocating each node just add extra memory for the data type.
eg:
    Data_Type data = ...
    Linked_List_Node* head = malloc(sizeof(Linked_List_Node) + sizeof(Data_Type) - sizeof(void*));
    memcpy(&(head->data), &data, sizeof(Data_Type));
    Data_Type* retrieved = (Data_Type*)(&head->data);

where '- sizeof(void*)' is used to subtract the size of the data pointer, which is replaced with the actual data.