#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <inttypes.h>

typedef struct Node {
    uint16_t data;
    struct Node * next;
} Node;

void list_init(Node ** head);
void list_insert(Node** head, int data);
void list_insert_after(Node* prev_node, int data);
void list_insert_before(Node** head, Node* next_node, int data);
void list_delete(Node** head, int data);
Node* list_search(Node* head, int data);
void list_display(Node** head, Node* start_node, Node* end_node);
int list_count_nodes(Node** head);
void list_cleanup(Node** head);

#endif