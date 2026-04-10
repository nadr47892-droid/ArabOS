#include "مدير_الخلفية.h"
#include "../محرك_الفيديو/محرك_الفيديو.h"

// متغير الحركة
static uint32_t time = 0;

void background_render() {
    uint32_t screen_w = fb_get_width();
    uint32_t screen_h = fb_get_height();

    for (uint32_t y = 0; y < screen_h; y++) {

        // نضيف الحركة هنا 🔥
        uint8_t ratio = ((y * 255) / screen_h + time) % 256;

        uint8_t red   = ratio;
        uint8_t green = 255 - ratio;
        uint8_t blue  = (ratio / 2) + 100;

        uint32_t color = (red << 16) | (green << 8) | blue;

        for (uint32_t x = 0; x < screen_w; x++) {
            fb_putpixel(x, y, color);
        }
    }

    // تحديث الزمن (سرعة الحركة)
    time += 1;
}
