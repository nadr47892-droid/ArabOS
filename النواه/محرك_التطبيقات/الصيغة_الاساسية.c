#include "الصيغة_الاساسية.h"

// تحقق من الهيدر
int app_format_check(void* data) {

    app_header_t* h = (app_header_t*)data;

    if (h->magic[0] != 'a' ||
        h->magic[1] != 'r' ||
        h->magic[2] != 'a' ||
        h->magic[3] != 'b') {
        return 0;
    }

    return 1;
}

// استخراج entry
void* app_get_entry(void* mem) {

    app_header_t* h = (app_header_t*)mem;
    return (uint8_t*)mem + h->entry;
}
