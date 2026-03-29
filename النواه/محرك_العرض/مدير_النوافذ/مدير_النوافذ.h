#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <stdint.h>

typedef struct {
    int x, y;
    int w, h;
    uint32_t color;
    int visible;
    const char* title;
    uint32_t* buffer;
    int is_maximized;
    int old_x, old_y;
    int old_w, old_h;
} window_t;

extern window_t windows[];
extern int window_count;

// API
int window_create(int x, int y, int w, int h, uint32_t color);
void window_move(int id, int dx, int dy);
void window_draw_content(window_t* win);
void window_manager_update();
void window_manager_init();

// جديد: الحصول على حجم الشاشة الحالي
uint32_t window_get_screen_width();
uint32_t window_get_screen_height();

// جديد: إعادة ضبط جميع النوافذ عند تغير حجم الشاشة
void window_reload_screen_size();

#endif
