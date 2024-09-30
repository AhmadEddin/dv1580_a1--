#include "linked_list.h"
#include "memory_manager.h"
#include <stdio.h>

void list_init(Node ** head){
    mem_init(2048);
    *head = NULL;
}
void list_insert(Node** head, int data){
    Node *new_node = (Node *)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed in list_insert\n");
        return;
    }
    new_node->data = (uint16_t)data;
    new_node->next = *head;

    *head = new_node;
}
void list_insert_after(Node* prev_node, int data){
    if (prev_node == NULL) {
        fprintf(stderr, "Error: The previous node cannot be NULL.\n");
        return;
    }
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed in list_insert_after\n");
        return;
    }
    new_node->data = (uint16_t)data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

void list_insert_before(Node** head, Node* next_node, int data){
    if (*head == NULL || next_node == NULL || head == NULL) {
        fprintf(stderr, "Invalid parameter.\n");
        return;
    }
    // If next_node is the head of the list, insert at the beginning.
    if (*head == next_node) {
        list_insert(head, data);
        return;
    }
    Node *current = *head;
    Node *prev = NULL;
    while (current != NULL && current != next_node) {
        prev = current;
        current = current->next;
    }
    if (current == NULL) {
        fprintf(stderr, "Error: The specified next node was not found in the list.\n");
        return;
    }
    
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed in list_insert_after\n");
        return;
    }
    new_node->data = (uint16_t)data;
    new_node->next = next_node;
    if (prev != NULL)
    {
        prev->next = new_node;
    }
    
    
}

void list_delete(Node** head, int data){
    if (head == NULL || *head == NULL) {
        fprintf(stderr, "Error: List is empty or head is NULL.\n");
        return;
    }

    Node *current = *head;
    Node *prev = NULL;

    while (current != NULL) {
        if (current->data == (uint16_t)data) {
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            mem_free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
    fprintf(stderr, "Error: Data %d not found in the list.\n", data);
}

Node* list_search(Node* head, int data){
    Node* current = head;
    while (current != NULL)
    {
        if (current->data == (uint16_t)data) {
            return current;
        }
        current = current->next;
    }
    return NULL;
    
}
void list_display(Node** head, Node* start_node, Node* end_node){
    printf("[");
    Node *current = head;
    while (current != NULL) {
        printf("%u", current->data);
        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]\n");
}

int list_count_nodes(Node** head){
    int count = 0;
    Node *current = head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
    
}

void list_cleanup(Node** head){
    if (head == NULL || *head == NULL) {
        return;
    }
    Node *current = *head;
    while (current != NULL) 
    {
        Node *next_node = current->next;
        mem_free(current);
        current = next_node;
    }
    *head = NULL;
    mem_deinit();
    
}
