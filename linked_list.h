#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <stdbool.h>

// Node structure for the singly linked list
typedef struct Node {
    uint16_t data;  // Stores the data (16-bit unsigned integer)
    struct Node* next;  // Pointer to the next node
} Node;

// Initializes the linked list by setting the head to NULL
void list_init(Node** head);

// Inserts a new node with the specified data at the end of the list
void list_insert(Node** head, int data);

// Inserts a new node with the specified data immediately after the given node
void list_insert_after(Node* prev_node, int data);

// Inserts a new node with the specified data immediately before the given node
void list_insert_before(Node** head, Node* next_node, int data);

// Deletes a node with the specified data from the list
void list_delete(Node** head, int data);

// Searches for a node with the specified data in the list
Node* list_search(Node* head, int data);

// Displays all the nodes in the list
void list_display(Node* head);

// Displays nodes within a specific range
void list_display_range(Node* head, Node* start_node, Node* end_node);

// Counts the number of nodes in the list
int list_count_nodes(Node* head);

// Frees all nodes in the list and sets the head pointer to NULL
void list_cleanup(Node* head);

#endif // LINKED_LIST_H
