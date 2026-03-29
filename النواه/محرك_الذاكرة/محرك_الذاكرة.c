#include "محرك_الذاكرة.h"

#define HEAP_START 0x01000000   // بداية heap
#define HEAP_SIZE  0x02000000   // 32MB

static unsigned char* heap = (unsigned char*)HEAP_START;
static size_t heap_used = 0;

void memory_init() {
    heap_used = 0;
}

void* kmalloc(size_t size) {

    size_t total_size = size + sizeof(size_t);

    if (heap_used + total_size >= HEAP_SIZE)
        return 0;

    //  احجز مكان للحجم
    size_t* header = (size_t*)(heap + heap_used);
    *header = size;

    void* ptr = (void*)(header + 1);

    heap_used += total_size;

    return ptr;
}

void kfree(void* ptr) {

    if (!ptr)
        return;

    //  ارجع للهيدر
    size_t* header = (size_t*)ptr - 1;

    size_t size = *header;

    //  إذا كان آخر عنصر (stack-like)
    if ((unsigned char*)ptr + size == heap + heap_used) {
        heap_used -= (size + sizeof(size_t));
    }

    // غير ذلك: لا نفعل شيء (حالياً)
}
