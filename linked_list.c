#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "memory_manager.h"


// Define a Node structure for the linked list
typedef struct Node {
    uint16_t data;  // Data field
    struct Node* next;  // Pointer to the next node
} Node;

// Initialize the linked list (set head to NULL)
void list_init(Node** head) {
    *head = NULL;
}


// Insert a new node at the end of the linked list
void list_insert(Node** head, int data) {
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation for new node failed.\n");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

// Delete a node from the linked list by value
void list_delete(Node** head, int data) {
    if (*head == NULL) {
        printf("List is empty.\n");
        return;
    }

    Node* temp = *head;
    Node* prev = NULL;

    // Find the node with the given data
    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Node with data %d not found.\n", data);
        return;
    }

    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }

    mem_free(temp);
}

// Display the contents of the linked list
void list_display(Node** head) {
    Node* temp = *head;
    printf("[");
    while (temp != NULL) {
        printf("%d", temp->data);
        temp = temp->next;
        if (temp != NULL) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Search for a node with the specified data
Node* list_search(Node** head, int data) {
    if (*head == NULL) {
        printf("List is empty.\n");
        return NULL;
    }
    Node* current = *head;
    
    // Traverse the list to find the node
    while (current != NULL) {
        if (current->data == data) {
            return current;  // Node found
        }
        current = current->next;
    }
    printf("Node with data %d not found.\n", data);
    return NULL;
}


// Cleanup the entire linked list
void list_cleanup(Node** head) {
    Node* current = *head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        mem_free(current);  // Free each node using custom memory manager
        current = next;
    }

    *head = NULL;
}

// Count the number of nodes in the linked list
int list_count_nodes(Node** head) {
    int count = 0;
    Node* temp = *head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}