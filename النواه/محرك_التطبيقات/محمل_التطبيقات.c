#include "محمل_التطبيقات.h"
#include "الصيغة_الاساسية.h"
#include "../محرك_الذاكرة/محرك_الذاكرة.h"

// ===== تحميل التطبيق =====
void* app_load(file_t* f) {

    if (!app_format_check(f->data))
        return 0;

    app_header_t* h = (app_header_t*)f->data;

    // حماية إضافية
    if (h->size == 0)
        return 0;

    void* mem = kmalloc(h->size);
    if (!mem) return 0;

    uint8_t* src = f->data;
    uint8_t* dst = mem;

    for (uint32_t i = 0; i < h->size; i++)
        dst[i] = src[i];

    return mem;
}
