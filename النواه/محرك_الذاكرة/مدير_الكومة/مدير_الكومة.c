#include "مدير_الكومة.h"
#include "مدير_الصفحات.h"

// ================== Config ==================
#define HEAP_START 0x40000000
#define PAGE_SIZE  4096
#define FRAME_START 0x100000

// ================== Heap State ==================
static uint64_t heap_current = HEAP_START;
static uint64_t heap_end     = HEAP_START;

static uint64_t next_free_frame = FRAME_START;

// ================== Init ==================
void heap_init() {
    heap_current = HEAP_START;
    heap_end     = HEAP_START;
}

// ================== Frame Allocator ==================
uint64_t alloc_frame() {

    uint64_t frame = next_free_frame;
    next_free_frame += PAGE_SIZE;

    return frame;
}

// ================== Heap Expand ==================
static void heap_expand() {

    uint64_t frame = alloc_frame();

    map_page(heap_end, frame);

    heap_end += PAGE_SIZE;
}

// ================== kmalloc ==================
void* kmalloc(size_t size) {

    if (size == 0)
        return NULL;

    // alignment 8 bytes
    if (size % 8 != 0)
        size += 8 - (size % 8);

    size_t total_size = size + sizeof(size_t);

    // تأكد أن هناك مساحة كافية
    while (heap_current + total_size > heap_end) {
        heap_expand();
    }

    // كتابة header
    size_t* header = (size_t*)heap_current;
    *header = size;

    void* ptr = (void*)(heap_current + sizeof(size_t));

    heap_current += total_size;

    return ptr;
}

// ================== kfree ==================
void kfree(void* ptr) {

    if (!ptr) return;

    // ⚠️ هذا heap لا يدعم free الحقيقي
    // فقط تجاهل العملية لتجنب الكراش

    (void)ptr;
}
