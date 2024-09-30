#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Define a block structure to track free/allocated blocks
typedef struct Block {
    size_t size;           // Size of the block
    struct Block* next;    // Pointer to the next block in the free list
    size_t free : 1;       // Store the free flag in a single bit
} Block;

// Global memory pool pointer and memory manager
static void* memory_pool = NULL;
static Block* free_list = NULL;

// Track total memory and used memory
static size_t total_memory_size = 0;
static size_t used_memory_size = 0;

// Minimum block size to include metadata
#define BLOCK_SIZE sizeof(Block)

// Function to initialize the memory pool
void mem_init(size_t size) {
    memory_pool = malloc(size + BLOCK_SIZE);  // Allocate extra space for the metadata
    if (memory_pool == NULL) {
        printf("Failed to initialize memory pool\n");
        return;
    }

    total_memory_size = size;
    used_memory_size = 0;

    // Initialize the first block with the entire pool minus the metadata size
    free_list = (Block*)memory_pool;
    free_list->size = size;  // Available memory for user allocations
    free_list->free = true;
    free_list->next = NULL;
}





// Function to find the first-fit block in the memory pool
Block* find_first_fit_block(size_t size) {
    Block* current = free_list;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Function to split a block if it's larger than requested size
void split_block(Block* block, size_t size) {
    if (block->size > size + BLOCK_SIZE) {
        Block* new_block = (Block*)((char*)block + BLOCK_SIZE + size);
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->free = true;
        new_block->next = block->next;

        block->size = size;
        block->next = new_block;
    }
}


// Function to allocate memory from the pool
void* mem_alloc(size_t requested_size) {
    Block* current = free_list;

    // Traverse the free list to find a suitable free block
    while (current != NULL) {
        // Check if the block is free and large enough
        if (current->free && current->size >= requested_size) {
            // Calculate the remaining space after allocation
            size_t remaining_space = current->size - requested_size;

            // If the block has enough space to be split
            if (remaining_space > sizeof(Block)) {
                // Create a new block for the remaining free space
                Block* new_block = (Block*)((char*)current + sizeof(Block) + requested_size);
                new_block->size = remaining_space - sizeof(Block);
                new_block->free = true;
                new_block->next = current->next;

                // Update the current block to reflect the allocated size
                current->next = new_block;
                current->size = requested_size;
            }

            // Mark the block as allocated
            current->free = false;

            // Return a pointer to the usable memory (just after the block's metadata)
            return (char*)current + sizeof(Block);
        }

        // Move to the next block
        current = current->next;
    }

    // No suitable block was found, return NULL
    return NULL;
}






// Function to coalesce adjacent free blocks
void coalesce() {
    Block* current = free_list;
    while (current != NULL && current->next != NULL) {
        if (current->free && current->next->free) {
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// Function to free allocated memory block
void mem_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->free = true;
    used_memory_size -= block->size + sizeof(Block);

    // Coalesce adjacent free blocks to avoid fragmentation
    coalesce();
}

// Function to resize an allocated block
void* mem_resize(void* ptr, size_t size) {
    if (ptr == NULL) {
        return mem_alloc(size);
    }

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    if (block->size >= size) {
        // No need to resize if current block is large enough
        split_block(block, size);
        return ptr;
    }

    void* new_ptr = mem_alloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size);  // Copy old data to new block
        mem_free(ptr);  // Free the old block
    }
    return new_ptr;
}

// Function to deinitialize the memory pool
void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;
    free_list = NULL;
    total_memory_size = 0;
    used_memory_size = 0;
}
