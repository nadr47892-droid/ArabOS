// محرك_التطبيقات.c
#include "محرك_التطبيقات.h"
#include "app_api.h"
#include "محمل_التطبيقات.h"
#include "الصيغة_الاساسية.h"

#include "../محرك_العمليات/محرك_العمليات.h"
#include "../محرك_الجدولة/محرك_الجدولة.h"
#include "../مدير_النوافذ/مدير_النوافذ.h"
#include "../محرك_العرض/مدير_النوافذ/مدير_عرض_النصوص/مدير_عرض_النصوص.h"

// ===== متغيرات عامة =====
static void (*global_entry)(app_api_t*);
static app_api_t global_api;
static int app_window_id = -1;

// ===== رسم مستطيل =====
static void app_draw_rect(int x, int y, int w, int h, uint32_t color) {
    if (app_window_id < 0) return;
    
    window_t* win = &windows[app_window_id];
    
    for (int i = 0; i < w && (x + i) < win->w; i++) {
        for (int j = 0; j < h && (y + j) < win->h; j++) {
            if (x + i >= 0 && y + j >= 0) {
                win->buffer[(y + j) * win->w + (x + i)] = color;
            }
        }
    }
}

// ===== رسم نص =====
static void app_draw_string(int x, int y, const char* str, uint32_t color) {
    if (app_window_id < 0) return;
    
    window_t* win = &windows[app_window_id];
    
    // استخدام draw_string_buffer من مدير_عرض_النصوص
    draw_string_buffer(win->buffer, win->w, x, y, str, color);
}

// ===== wrapper =====
static void app_entry_wrapper() {
    global_entry(&global_api);
    while (1) {
        for(volatile int i = 0; i < 100000; i++);
    }
}

// ===== تشغيل التطبيق =====
void app_run(file_t* f) {
    void* mem = app_load(f);
    if (!mem) return;
    
    // إنشاء نافذة جديدة للتطبيق (مختلفة عن نافذة الطرفية)
    app_window_id = window_create(50, 50, 400, 300, 0x001133);
    
    if (app_window_id < 0) return;
    
    // إعداد API
    global_entry = app_get_entry(mem);
    global_api.draw_rect = app_draw_rect;
    global_api.draw_string = app_draw_string;
    
    // إنشاء عملية
    process_t* p = process_create(app_entry_wrapper);
    if (p) {
        scheduler_add(p);
    }
}
