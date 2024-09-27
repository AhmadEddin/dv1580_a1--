#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include <stddef.h>

// Define the Node structure for the singly linked list
typedef struct Node {
    uint16_t data; // Data field (16-bit unsigned integer)
    struct Node* next; // Pointer to the next node
} Node;

// Function declarations for linked list operations

// Initializes the linked list by allocating memory for 'size' number of nodes
void list_init(Node** head, size_t size);

// Inserts a new node with the specified data at the end of the list
void list_insert(Node** head, uint16_t data);

// Inserts a new node with the specified data after the specified node
void list_insert_after(Node* prev_node, uint16_t data);

// Inserts a new node with the specified data before the specified node
void list_insert_before(Node** head, Node* next_node, uint16_t data);

// Deletes the first node with the specified data from the list
void list_delete(Node** head, uint16_t data);

// Searches for a node with the specified data and returns a pointer to it
Node* list_search(Node** head, uint16_t data);

// Displays all the nodes in the list
void list_display(Node** head);

// Displays the nodes in a specific range, starting from start_node and ending at end_node
void list_display_range(Node** head, Node* start_node, Node* end_node);

// Counts the number of nodes in the list
int list_count_nodes(Node** head);

// Frees all the nodes in the list and deallocates the memory
void list_cleanup(Node** head);

#endif // LINKED_LIST_H
