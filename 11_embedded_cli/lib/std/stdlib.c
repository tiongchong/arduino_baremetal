#include "stdlib.h"
#include "std/stdint.h"

// Simple heap implementation for embedded systems
// This is a very basic implementation - for production use, consider a proper heap manager

#define HEAP_SIZE 1024
static char heap[HEAP_SIZE];
static size_t heap_used = 0;

void* malloc(size_t size) {
    // Simple linear allocator - no free() support
    if (heap_used + size > HEAP_SIZE) {
        return 0; // Out of memory
    }
    void* ptr = &heap[heap_used];
    heap_used += size;
    return ptr;
}

void free(void* ptr) {
    // No-op for simple allocator
    (void)ptr;
}

void* realloc(void* ptr, size_t size) {
    // Simple implementation - just allocate new block
    // In a real implementation, you'd try to extend the existing block
    void* new_ptr = malloc(size);
    if (new_ptr && ptr) {
        // Copy old data (we don't know the old size, so this is simplified)
        // In practice, you'd need to track allocation sizes
    }
    return new_ptr;
}

void* calloc(size_t num, size_t size) {
    void* ptr = malloc(num * size);
    if (ptr) {
        // Zero the memory
        unsigned char* p = (unsigned char*)ptr;
        for (size_t i = 0; i < num * size; i++) {
            p[i] = 0;
        }
    }
    return ptr;
}

