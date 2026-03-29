#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 1024

typedef struct {
    uint64_t entries[PAGE_ENTRIES];
} page_table_t;

typedef struct {
    uint64_t entries[PAGE_ENTRIES];
} page_directory_t;

void paging_init();
void paging_enable(page_directory_t* dir);

#endif
