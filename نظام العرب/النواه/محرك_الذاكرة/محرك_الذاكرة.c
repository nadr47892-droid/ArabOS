#include "محرك_الذاكرة.h"

#define HEAP_START 0x01000000   // بداية heap
#define HEAP_SIZE  0x00100000   // 1MB

static unsigned char* heap = (unsigned char*)HEAP_START;
static size_t heap_used = 0;

void memory_init() {
    heap_used = 0;
}

void* kmalloc(size_t size) {

    if (heap_used + size >= HEAP_SIZE)
        return 0;

    void* ptr = heap + heap_used;

    heap_used += size;

    return ptr;
}

void kfree(void* ptr) {

    if (!ptr)
        return;

}
