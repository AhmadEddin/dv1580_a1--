#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Define a block structure to track free/allocated blocks
typedef struct Block {
    size_t size;           // Size of the block (excluding metadata)
    bool free;             // Is the block free?
    struct Block* next;    // Pointer to the next block in the free list
} Block;

// Global memory pool pointer and memory manager
static void* memory_pool = NULL;
static Block* free_list = NULL;

// Track used memory size
static size_t used_memory_size = 0;

// Minimum block size to include metadata
#define BLOCK_SIZE sizeof(Block)
#define MIN_SPLIT_SIZE 16  // Minimum size for a block after splitting

// Initialize memory pool
void mem_init(size_t size) {
    memory_pool = malloc(size + BLOCK_SIZE);  // Allocate memory for the pool (including metadata)
    if (memory_pool == NULL) {
        perror("Memory pool initialization failed");
        return;
    }

    // Set up the initial block to be the entire memory pool
    free_list = (Block*)memory_pool;
    free_list->size = size;  // The size of the usable memory
    free_list->free = true;
    free_list->next = NULL;

    used_memory_size = 0;
    printf("Memory pool initialized with %zu bytes.\n", size);
}

// Allocate memory from the pool
void* mem_alloc(size_t requested_size) {
    printf("Attempting to allocate %zu bytes\n", requested_size);
    Block* current = free_list;

    while (current != NULL) {
        printf("Checking block at %p: size=%zu, is_free=%d\n", (void*)current, current->size, current->free);

        if (current->free && current->size >= requested_size) {
            size_t remaining_size = current->size - requested_size;

            // If there's enough space to split the block
            if (remaining_size > sizeof(Block)) {
                Block* new_block = (Block*)((char*)current + requested_size + sizeof(Block));
                new_block->size = remaining_size;
                new_block->free = true;
                new_block->next = current->next;

                current->next = new_block;
                current->size = requested_size;
            } else {
                requested_size = current->size;  // Allocate the whole block
            }

            current->free = false;
            printf("Allocated %zu bytes at %p\n", requested_size, (void*)current);

            // Return pointer to usable memory
            return (char*)current + sizeof(Block);
        }

        current = current->next;
    }

    printf("Memory allocation failed: no suitable block found.\n");
    return NULL;
}

// Free allocated memory block
void mem_free(void* block) {
    if (block == NULL) {
        printf("Attempt to free NULL pointer.\n");
        return;
    }

    // Find the block metadata by subtracting the size of the Block struct
    Block* block_to_free = (Block*)((char*)block - BLOCK_SIZE);

    // Check if the block is already free (double free check)
    if (block_to_free->free) {
        printf("Error: Attempt to free an already freed block at %p.\n", (void*)block_to_free);
        return;
    }

    block_to_free->free = true;
    used_memory_size -= block_to_free->size + BLOCK_SIZE;

    printf("Freed block at %p\n", (void*)block_to_free);

    // Coalesce adjacent free blocks to avoid fragmentation
    Block* current = free_list;
    while (current != NULL && current->next != NULL) {
        // Check if current block and next block are both free
        if (current->free && current->next->free) {
            // Ensure that the blocks are contiguous in memory
            if ((char*)current + current->size + BLOCK_SIZE == (char*)current->next) {
                // Merge the two blocks
                current->size += current->next->size + BLOCK_SIZE;
                current->next = current->next->next;  // Update the next pointer
                printf("Merged adjacent free blocks at %p, new size: %zu bytes.\n", (void*)current, current->size);
            }
        }
        current = current->next;
    }
}

// Resize an allocated block
void* mem_resize(void* ptr, size_t size) {
    if (ptr == NULL) {
        return mem_alloc(size);  // Allocate new memory if the pointer is NULL
    }

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    if (block->size >= size) {
        return ptr;  // If the current block is large enough, return it
    }

    void* new_ptr = mem_alloc(size);  // Allocate a new block
    if (new_ptr == NULL) {
        return NULL;  // Allocation failed
    }

    memcpy(new_ptr, ptr, block->size);  // Copy the data to the new block
    mem_free(ptr);  // Free the old block
    return new_ptr;
}

// Deinitialize the memory pool
void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;
    free_list = NULL;
    used_memory_size = 0;
    printf("Memory pool deinitialized.\n");
}
