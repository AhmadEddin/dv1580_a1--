#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Define a block structure to track free/allocated blocks
typedef struct Block {
    size_t size;           // Size of the block
    bool free;             // Is the block free?
    struct Block* next;    // Pointer to the next block in the free list
} Block;

// Global memory pool pointer and memory manager
static void* memory_pool = NULL;
static Block* free_list = NULL;

// Track total memory and used memory
static size_t total_memory_size = 0;
static size_t used_memory_size = 0;

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

    total_memory_size = size;
    used_memory_size = 0;
}

// Find the best-fit block by traversing the free list
Block* find_best_fit_block(size_t size) {
    Block* best_fit = NULL;
    Block* current = free_list;

    // Traverse the free list to find a suitable block
    while (current != NULL) {
        // Check if the block is free and has enough space for the requested size
        if (current->free && current->size >= size) {
            if (best_fit == NULL || current->size < best_fit->size) {
                best_fit = current;
            }
        }
        current = current->next;
    }

    return best_fit;
}

// Allocate memory from the pool
void* mem_alloc(size_t size) {
    // Ensure the requested size is non-zero
    if (size == 0) {
        printf("Requested memory size is zero.\n");
        return NULL;
    }

    // Check if there's enough space left in the memory pool
    if (used_memory_size + size + BLOCK_SIZE > total_memory_size) {
        printf("Memory allocation exceeds total available memory.\n");
        return NULL;
    }

    // Find the best-fit block using the best-fit strategy
    Block* block = find_best_fit_block(size);

    // If no suitable block was found, return NULL
    if (block == NULL) {
        printf("No suitable block found for allocation.\n");
        return NULL;
    }

    // Calculate the remaining size after allocation
    size_t remaining_size = block->size - size;

    // If there is enough space to split the block
    if (remaining_size > BLOCK_SIZE) {
        Block* new_block = (Block*)((char*)block + BLOCK_SIZE + size);  // Split the block
        new_block->size = remaining_size - BLOCK_SIZE;
        new_block->free = true;
        new_block->next = block->next;
        block->next = new_block;
        block->size = size;  // Resize the current block
    } else {
        // Allocate the entire block if it cannot be split
        remaining_size = 0;
    }

    // Mark the current block as allocated
    block->free = false;

    // Update the used memory size
    used_memory_size += block->size + BLOCK_SIZE;

    // Return a pointer to the usable memory (just after the block metadata)
    return (char*)block + BLOCK_SIZE;
}

// Free allocated memory block
void mem_free(void* ptr) {
    if (ptr == NULL) return;

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    block->free = true;

    // Update used memory size
    used_memory_size -= block->size + BLOCK_SIZE;

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
    total_memory_size = 0;
    used_memory_size = 0;
}
