#ifndef APP_FORMAT_H
#define APP_FORMAT_H

#include <stdint.h>

// ===== صيغة التطبيق =====
typedef struct {
    char magic[4];     // "arab"
    uint32_t entry;    // offset
    uint32_t size;     // الحجم الكلي
} app_header_t;

// ===== دوال =====
int app_format_check(void* data);
void* app_get_entry(void* mem);

#endif
