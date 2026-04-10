#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

void heap_init();

void* kmalloc(size_t size);
void kfree(void* ptr);

uint64_t alloc_frame();

#endif
