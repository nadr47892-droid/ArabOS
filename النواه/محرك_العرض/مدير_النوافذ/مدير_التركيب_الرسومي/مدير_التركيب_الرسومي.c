#include "مدير_التركيب_الرسومي.h"
#include "../محرك_الفيديو/محرك_الفيديو.h"
#include "../محرك_الفيديو/عارض_الصورة.h"
#include "مدير_الخلفية.h"

extern window_t windows[];
extern int window_count;

// رسم نافذة داخل back buffer
static void blit_window(window_t* win) {

    uint32_t screen_w = fb_get_width();
    uint32_t screen_h = fb_get_height();
    uint32_t pitch    = fb_get_pitch();   // ✅ مهم جدًا

    uint32_t* back = fb_get_back_buffer();

    if (!win->buffer) return; // 🛡 حماية

    for (int y = 0; y < win->h; y++) {

        int screen_y = win->y + y;
        if (screen_y < 0 || screen_y >= (int)screen_h)
            continue;

        for (int x = 0; x < win->w; x++) {

            int screen_x = win->x + x;
            if (screen_x < 0 || screen_x >= (int)screen_w)
                continue;

            uint32_t color = win->buffer[y * win->w + x];

            // ✅ استخدام pitch بدل width
            back[screen_y * pitch + screen_x] = color;
        }
    }
}

void compositor_render() {

    uint32_t screen_w = fb_get_width();
    uint32_t screen_h = fb_get_height();
    uint32_t pitch    = fb_get_pitch();   // ✅ مهم جدًا

    uint32_t* back = fb_get_back_buffer();

    if (!back) return; // 🛡 حماية

    // 🧹 1. مسح الشاشة
    for (uint32_t y = 0; y < screen_h; y++) {
        for (uint32_t x = 0; x < screen_w; x++) {
            back[y * pitch + x] = 0x00000000;
        }
    }

    // 🎨 2. الخلفية
    background_render();

    // 🪟 3. رسم النوافذ
    for (int i = 0; i < window_count; i++) {

        if (!windows[i].visible)
            continue;

        blit_window(&windows[i]);
    }
}
