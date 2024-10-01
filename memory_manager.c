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
void* mem_alloc(size_t size) {
    // If requested size is zero, return the next available block without allocating
    if (size == 0) {
        printf("Requested memory size is zero. Returning a pointer to the next available block.\n");

        // Traverse the free list to find the first free block and return its pointer
        Block* current = free_list;
        while (current != NULL) {
            if (current->free) {
                printf("Returning block at %p for zero-sized allocation.\n", (void*)current);
                return (char*)current + BLOCK_SIZE;  // Return pointer to usable memory
            }
            current = current->next;
        }

        printf("No free block available for zero-sized allocation.\n");
        return NULL;
    }

    // Print memory allocation attempt
    printf("Attempting to allocate %zu bytes.\n", size);

    Block* current = free_list;

    // Print the initial state of the free list
    printf("Initial free list:\n");
    Block* temp = free_list;
    while (temp != NULL) {
        printf("Block at %p: size = %zu, free = %d\n", (void*)temp, temp->size, temp->free);
        temp = temp->next;
    }

    // Traverse the free list to find the first suitable block
    while (current != NULL) {
        printf("Checking block at %p: size = %zu, free = %d\n", (void*)current, current->size, current->free);

        if (current->free && current->size >= size) {
            printf("Found suitable block at %p: size = %zu\n", (void*)current, current->size);

            size_t remaining_size = current->size - size;
            printf("Remaining size after allocation: %zu\n", remaining_size);

            // If there is enough space to split the block
            if (remaining_size >= MIN_SPLIT_SIZE) {
                printf("Splitting block at %p\n", (void*)current);

                Block* new_block = (Block*)((char*)current + BLOCK_SIZE + size);
                new_block->size = remaining_size;
                new_block->free = true;
                new_block->next = current->next;

                current->next = new_block;
                current->size = size;  // Resize the current block to the requested size

                printf("Created new block at %p: size = %zu\n", (void*)new_block, new_block->size);
            } else {
                // If remaining size is too small, allocate the entire block
                printf("Allocating the entire block at %p (no split)\n", (void*)current);
                size = current->size;  // Adjust requested size to the full block size
            }

            // Mark the current block as used
            current->free = false;
            used_memory_size += current->size + BLOCK_SIZE;

            // Print memory allocation details
            printf("Allocated %zu bytes at %p\n", size, (void*)current);

            // Return a pointer to the usable memory (just after the block metadata)
            return (char*)current + BLOCK_SIZE;
        }

        // Move to the next block in the list
        printf("Block at %p is either not free or too small\n", (void*)current);
        current = current->next;
    }

    // If no suitable block was found, return NULL
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
    block_to_free->free = true;
    used_memory_size -= block_to_free->size + BLOCK_SIZE;

    printf("Freed block at %p\n", (void*)block_to_free);

    // Coalesce adjacent free blocks to avoid fragmentation
    Block* current = free_list;
    while (current != NULL && current->next != NULL) {
        if (current->free && current->next->free) {
            current->size += current->next->size + BLOCK_SIZE;
            current->next = current->next->next;
            printf("Merged adjacent free blocks at %p, new size: %zu bytes.\n", (void*)current, current->size);
        } else {
            current = current->next;
        }
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
