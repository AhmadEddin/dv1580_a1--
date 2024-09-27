#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Define a block structure to track free/allocated blocks
typedef struct Block {
    size_t size;           // Size of the block
    bool free;             // Is the block free?
    struct Block* next;    // Pointer to the next block
} Block;

// Global memory pool pointer and memory manager
static void* memory_pool = NULL;
static Block* free_list = NULL;

// Minimum block size to include metadata
#define BLOCK_SIZE sizeof(Block)

// Initialize memory pool
void mem_init(size_t size) {
    memory_pool = malloc(size);
    if (memory_pool == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    // Initializing the whole pool as one free block
    free_list = (Block*)memory_pool;
    free_list->size = size - BLOCK_SIZE;  // Exclude block metadata
    free_list->free = true;
    free_list->next = NULL;
}

// Find a suitable block using the first-fit strategy
Block* find_free_block(size_t size) {
    Block* current = free_list;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Allocate memory from the pool
void* mem_alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    Block* block = find_free_block(size);
    if (block == NULL) {
        printf("No suitable block found for allocation.\n");
        return NULL;
    }

    // If the block is larger than required, split it
    if (block->size >= size + BLOCK_SIZE) {  // Ensure enough space to split
        Block* new_block = (Block*)((char*)block + BLOCK_SIZE + size);
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->free = true;
        new_block->next = block->next;
        block->next = new_block;
        block->size = size;
    }

    block->free = false;  // Mark this block as allocated
    return (char*)block + BLOCK_SIZE;  // Return memory address after block metadata
}

// Free allocated memory block
void mem_free(void* ptr) {
    if (ptr == NULL) return;

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    block->free = true;

    // Merge adjacent free blocks
    Block* current = free_list;
    while (current != NULL) {
        if (current->free && current->next != NULL && current->next->free) {
            current->size += current->next->size + BLOCK_SIZE;
            current->next = current->next->next;
        }
        current = current->next;
    }
}

// Resize an allocated block
void* mem_resize(void* ptr, size_t size) {
    if (ptr == NULL) {
        return mem_alloc(size);  // If the block is NULL, allocate new memory
    }

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    if (block->size >= size) {
        return ptr;  // If current block is large enough, keep it
    }

    void* new_ptr = mem_alloc(size);  // Allocate new block
    if (new_ptr == NULL) {
        return NULL;  // Failed to allocate
    }

    memcpy(new_ptr, ptr, block->size);  // Copy old data to new block
    mem_free(ptr);  // Free the old block
    return new_ptr;
}

// Deinitialize the memory pool
void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;
    free_list = NULL;
}
