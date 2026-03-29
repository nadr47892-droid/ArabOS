#include <stdint.h>
#include "مدير_النوافذ.h"
#include "../../محرك_الذاكرة/محرك_الذاكرة.h"
#include "../../محرك_الفيديو/محرك_الفيديو.h"
#include "مدير_عرض_النصوص.h"

#define MAX_WINDOWS 10

window_t windows[MAX_WINDOWS];
int window_count = 0;

extern int mouse_x;
extern int mouse_y;
extern int mouse_left;

static int dragging = -1;
static int offset_x = 0;
static int offset_y = 0;

// متغيرات ديناميكية لحجم الشاشة
static uint32_t screen_width = 0;
static uint32_t screen_height = 0;
static uint32_t screen_pitch = 0;

// الحصول على حجم الشاشة الحالي
uint32_t window_get_screen_width() { return screen_width; }
uint32_t window_get_screen_height() { return screen_height; }

// تهيئة مدير النوافذ مع حجم الشاشة الحالي
void window_manager_init() {
    screen_width = fb_get_width();
    screen_height = fb_get_height();
    screen_pitch = fb_get_pitch();
}

// إعادة تحميل حجم الشاشة (للحالات التي قد يتغير فيها حجم الشاشة)
void window_reload_screen_size() {
    uint32_t new_width = fb_get_width();
    uint32_t new_height = fb_get_height();
    
    if (new_width != screen_width || new_height != screen_height) {
        screen_width = new_width;
        screen_height = new_height;
        screen_pitch = fb_get_pitch();
        
        // إعادة ضبط النوافذ المكبرة
        for (int i = 0; i < window_count; i++) {
            if (windows[i].is_maximized) {
                // إعادة ضبط النافذة المكبرة للحجم الجديد
                windows[i].w = screen_width;
                windows[i].h = screen_height;
                windows[i].x = 0;
                windows[i].y = 0;
                
                // إعادة تخصيص البفر
                kfree(windows[i].buffer);
                windows[i].buffer = kmalloc(windows[i].w * windows[i].h * sizeof(uint32_t));
                if (windows[i].buffer) {
                    window_draw_content(&windows[i]);
                }
            } else {
                // التأكد من أن النوافذ العادية لا تخرج عن الشاشة
                if (windows[i].x + windows[i].w > (int)screen_width) {
                    windows[i].x = screen_width - windows[i].w;
                    if (windows[i].x < 0) windows[i].x = 0;
                }
                if (windows[i].y + windows[i].h > (int)screen_height) {
                    windows[i].y = screen_height - windows[i].h;
                    if (windows[i].y < 0) windows[i].y = 0;
                }
            }
        }
    }
}

int window_create(int x, int y, int w, int h, uint32_t color) {
    if (window_count >= MAX_WINDOWS)
        return -1;

    // التأكد من أن النافذة داخل حدود الشاشة
    if (x + w > (int)screen_width) {
        w = screen_width - x;
        if (w <= 0) return -1;
    }
    if (y + h > (int)screen_height) {
        h = screen_height - y;
        if (h <= 0) return -1;
    }

    windows[window_count].x = x;
    windows[window_count].y = y;
    windows[window_count].w = w;
    windows[window_count].h = h;
    windows[window_count].color = color;
    windows[window_count].visible = 1;
    windows[window_count].title = "ArabOS";
    windows[window_count].is_maximized = 0;

    windows[window_count].buffer =
        kmalloc(w * h * sizeof(uint32_t));

    if (!windows[window_count].buffer) {
        return -1;
    }

    return window_count++;
}

void window_move(int id, int dx, int dy) {
    if (id < 0 || id >= window_count) return;
    
    int new_x = windows[id].x + dx;
    int new_y = windows[id].y + dy;
    
    // التأكد من عدم خروج النافذة عن الشاشة
    if (new_x >= 0 && new_x + windows[id].w <= (int)screen_width) {
        windows[id].x = new_x;
    }
    if (new_y >= 0 && new_y + windows[id].h <= (int)screen_height) {
        windows[id].y = new_y;
    }
}

void window_manager_update() {
    if (mouse_left) {
        if (dragging == -1) {
            for (int i = window_count - 1; i >= 0; i--) {
                if (!windows[i].visible)
                    continue;

                // زر الإغلاق
                int btn_x = windows[i].x + windows[i].w - 14;
                int btn_y = windows[i].y + 2;

                if (mouse_x >= btn_x &&
                    mouse_x < btn_x + 12 &&
                    mouse_y >= btn_y &&
                    mouse_y < btn_y + 12) {
                    windows[i].visible = 0;
                    return;
                }

                // زر التكبير - يأخذ ملء الشاشة تلقائياً
                int max_x = windows[i].x + windows[i].w - 28;
                int max_y = windows[i].y + 2;

                if (mouse_x >= max_x &&
                    mouse_x < max_x + 12 &&
                    mouse_y >= max_y &&
                    mouse_y < max_y + 12) {

                    window_t* w = &windows[i];

                    if (!w->is_maximized) {
                        w->old_x = w->x;
                        w->old_y = w->y;
                        w->old_w = w->w;
                        w->old_h = w->h;

                        w->x = 0;
                        w->y = 0;
                        w->w = screen_width;
                        w->h = screen_height;

                        w->is_maximized = 1;
                    } else {
                        w->x = w->old_x;
                        w->y = w->old_y;
                        w->w = w->old_w;
                        w->h = w->old_h;
                        w->is_maximized = 0;
                    }

                    kfree(w->buffer);
                    uint32_t* new_buffer = kmalloc(w->w * w->h * sizeof(uint32_t));

                    if (!new_buffer) {
                        if (w->is_maximized) {
                            w->is_maximized = 0;
                            w->x = w->old_x;
                            w->y = w->old_y;
                            w->w = w->old_w;
                            w->h = w->old_h;
                        } else {
                            w->is_maximized = 1;
                            w->x = 0;
                            w->y = 0;
                            w->w = screen_width;
                            w->h = screen_height;
                        }
                        
                        new_buffer = kmalloc(w->w * w->h * sizeof(uint32_t));
                        if (!new_buffer) return;
                    }

                    w->buffer = new_buffer;
                    window_draw_content(w);
                    return;
                }

                // تحريك النافذة
                if (mouse_x >= windows[i].x &&
                    mouse_x < windows[i].x + windows[i].w &&
                    mouse_y >= windows[i].y &&
                    mouse_y < windows[i].y + 18) {

                    dragging = i;
                    offset_x = mouse_x - windows[i].x;
                    offset_y = mouse_y - windows[i].y;
                    break;
                }
            }
        }
    } else {
        dragging = -1;
    }

    if (dragging != -1 && !windows[dragging].is_maximized) {
        int new_x = mouse_x - offset_x;
        int new_y = mouse_y - offset_y;
        
        // منع خروج النافذة عن الشاشة
        if (new_x >= 0 && new_x + windows[dragging].w <= (int)screen_width) {
            windows[dragging].x = new_x;
        }
        if (new_y >= 0 && new_y + windows[dragging].h <= (int)screen_height) {
            windows[dragging].y = new_y;
        }
    }
}

void window_draw_content(window_t* win) {
    // خلفية النافذة
    for (int y = 0; y < win->h; y++) {
        for (int x = 0; x < win->w; x++) {
            win->buffer[y * win->w + x] = win->color;
        }
    }

    // رسم نص داخل النافذة
    draw_string_buffer(
        win->buffer,
        win->w,
        10, 25,
        "ArabOS--AlArab OS",
        0xFFFFFF
    );
}
