#include "مدير_الصفحات.h"

page_directory_t* kernel_directory;
page_table_t first_table;

void paging_init() {

    kernel_directory = (page_directory_t*)0x1000;
    page_table_t* first_table = (page_table_t*)0x2000;

    for (int i = 0; i < PAGE_ENTRIES; i++) {
        first_table->entries[i] = (i * PAGE_SIZE) | 3;
    }

    kernel_directory->entries[0] = ((uintptr_t)first_table) | 3;
}
