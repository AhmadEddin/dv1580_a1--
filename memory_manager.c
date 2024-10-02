#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "memory_manager.h"
#include <assert.h>
#include <errno.h>
#include "common_defs.h"

// Structure for memory blocks in the pool
typedef struct Block {
    size_t size;        // Size of the block (usable memory)
    bool is_free;       // Block status (true if free, false if allocated)
    struct Block* next; // Pointer to the next block in the memory pool
} Block;

void* memory_pool = NULL;       // Pointer to the start of the memory pool
size_t memory_pool_size = 0;    // Total size of the memory pool
Block* free_list = NULL;        // Linked list of free blocks in the memory pool

/**
 * Initializes the memory pool.
 * 
 * @param size: The total size of the memory pool to be initialized.
 * 
 * This function allocates memory for the memory pool and sets up the first block
 * in the free list. If memory allocation fails, the function returns without
 * further action.
 */
void mem_init(size_t size) {
    memory_pool = malloc(size);  // Allocate memory for the pool
    if (!memory_pool) {
        printf("Memory pool allocation failed\n");
        return;
    }
    
    memory_pool_size = size;
    
    // Initialize the first block
    free_list = (Block*)memory_pool;
    free_list->size = size;  // Block size minus header size
    free_list->is_free = true;
    free_list->next = NULL;
}

/**
 * Allocates a block of memory from the memory pool.
 * 
 * @param requested_size: The size of memory to be allocated.
 * 
 * @return: Pointer to the allocated memory, or NULL if allocation fails.
 */
void* mem_alloc(size_t requested_size) {
    Block* current = free_list;

    if (requested_size == 0) {
        // If requested size is 0, return the first available free block's data pointer
        // but don't actually mark it as allocated or split it.
        printf("Allocating minimal block for 0 bytes request\n");
        return (void*)((char*)current + sizeof(Block)); // Return pointer to first block's data
    }

    // Align the requested size to ensure proper memory alignment
    requested_size = (requested_size + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
    printf("Requested size: %zu\n", requested_size);

    while (current != NULL) {
        printf("Current block size: %zu, is_free: %d\n", current->size, current->is_free);
        
        if (current->is_free && current->size >= requested_size) {
            // Calculate remaining size after allocation
            size_t remaining_size = current->size - requested_size;

            if (remaining_size > sizeof(Block)) {
                // Create a new block from the remaining memory
                Block* new_block = (Block*)((char*)current + sizeof(Block) + requested_size);
                new_block->size = remaining_size;
                new_block->is_free = true;
                new_block->next = current->next;
                current->next = new_block;
                
                // Update the current block's size and mark it as allocated
                current->size = requested_size;
            }
            
            current->is_free = false;
            printf("Allocated block of size: %zu\n", current->size);
            return (void*)((char*)current + sizeof(Block));
        }
        current = current->next;
    }

    printf("No suitable block found for allocation\n");
    return NULL;  // No suitable block found
}


/**
 * Frees a previously allocated block of memory.
 * 
 * @param block: The pointer to the memory block to be freed.
 */
void mem_free(void* block) {
    if (!block) return;

    Block* header = (Block*)((char*)block - sizeof(Block));
    header->is_free = true;

    // Coalesce adjacent free blocks
    Block* current = free_list;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(Block) + current->next->size;
            current->next = current->next->next;
        }
        current = current->next;
    }
}

/**
 * Resizes an allocated memory block.
 * 
 * @param block: The pointer to the memory block to be resized.
 * @param size: The new size of the memory block.
 * 
 * @return: Pointer to the resized memory block, or NULL if resizing fails.
 */
void* mem_resize(void* block, size_t size) {
    if (!block) {
        return mem_alloc(size);  // If block is NULL, allocate a new block
    }

    Block* header = (Block*)((char*)block - sizeof(Block));

    if (header->size >= size) {
        return block;  // No need to resize if the current block is large enough
    }

    // Allocate a new block and copy the old content
    void* new_block = mem_alloc(size);
    if (new_block) {
        memcpy(new_block, block, header->size);  // Copy old content to new block
        mem_free(block);  // Free the old block
    }

    return new_block;
}

/**
 * De-initializes the memory pool.
 * 
 * This function frees the memory pool and resets all related variables.
 */
void mem_deinit() {
    free(memory_pool);  // Free the memory pool
    memory_pool = NULL;
    memory_pool_size = 0;
    free_list = NULL;
}
