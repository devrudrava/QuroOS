#include "../../include/string.h"
#include <stdint.h>

#define HEAP_START 0x200000
#define HEAP_SIZE  0x600000

void memory_initialize(void) {
    uint8_t* heap = (uint8_t*)HEAP_START;
    memset(heap, 0, HEAP_SIZE);
}

void* malloc(size_t size) {
    // Simple bump allocator
    static uint8_t* heap_ptr = (uint8_t*)HEAP_START;
    if (size == 0) return NULL;
    size = (size + 7) & ~7u;
    if (heap_ptr + size > (uint8_t*)(HEAP_START + HEAP_SIZE)) return NULL;
    void* ptr = heap_ptr;
    heap_ptr += size;
    return ptr;
}

void free(void* ptr) {
    (void)ptr; // Simple allocator doesn't free
}
