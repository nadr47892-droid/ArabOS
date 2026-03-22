#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void memory_init();

void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
