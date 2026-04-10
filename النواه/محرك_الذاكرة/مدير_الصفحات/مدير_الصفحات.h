#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_PRESENT  1
#define PAGE_WRITABLE 2
#define PAGE_HUGE     (1 << 7)

#define ENTRIES 512

void paging_init(uint64_t fb_addr);
void paging_enable(uint64_t* pml4);
uint64_t* paging_get_pml4();

void map_page(uint64_t virt, uint64_t phys);


#endif
