#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "memory_manager.h"
#include "linked_list.h"

// Initialize the linked list by allocating memory for 'size' number of nodes
void list_init(Node** head, size_t size) {
    *head = (Node*)mem_alloc(sizeof(Node) * size);
    if (*head == NULL) {
        printf("Memory allocation failed during list initialization.\n");
    }
}

// Insert a new node at the end of the linked list
void list_insert(Node** head, uint16_t data) {
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

// Insert a new node after a given node
void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        printf("Previous node cannot be NULL.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

// Insert a new node before a given node
void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (*head == NULL || next_node == NULL) {
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    new_node->data = data;

    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
    } else {
        Node* temp = *head;
        while (temp != NULL && temp->next != next_node) {
            temp = temp->next;
        }

        if (temp != NULL) {
            new_node->next = next_node;
            temp->next = new_node;
        } else {
            printf("Next node not found in the list.\n");
            mem_free(new_node);  // Free the memory if insertion fails
        }
    }
}

// Delete the first node with the specified data from the linked list
void list_delete(Node** head, uint16_t data) {
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

    // If node with data is not found
    if (temp == NULL) {
        printf("Node with data %d not found.\n", data);
        return;
    }

    // If node to be deleted is the head
    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
    }

    mem_free(temp);  // Free the memory of the node being deleted
}

// Search for a node with the specified data and return a pointer to it
Node* list_search(Node** head, uint16_t data) {
    Node* current = *head;

    // Traverse the list to find the node
    while (current != NULL) {
        if (current->data == data) {
            return current;  // Node found
        }
        current = current->next;
    }

    printf("Node with data %d not found.\n", data);
    return NULL;  // Node not found
}

// Display all nodes in the linked list
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

// Display the nodes in a range (from start_node to end_node)
void list_display_range(Node** head, Node* start_node, Node* end_node) {
    Node* current = (start_node == NULL) ? *head : start_node;

    printf("[");
    while (current != NULL) {
        printf("%d", current->data);
        if (current == end_node) {
            break;
        }
        current = current->next;
        if (current != NULL && current != end_node) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Count the number of nodes in the list
int list_count_nodes(Node** head) {
    int count = 0;
    Node* temp = *head;

    while (temp != NULL) {
        count++;
        temp = temp->next;
    }

    return count;
}

// Free all nodes in the linked list
void list_cleanup(Node** head) {
    Node* current = *head;
    Node* next_node;

    while (current != NULL) {
        next_node = current->next;
        mem_free(current);  // Use custom memory manager to free memory
        current = next_node;
    }

    *head = NULL;  // Set head to NULL after cleanup
}
