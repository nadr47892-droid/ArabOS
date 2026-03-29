#include "عارض_الصورة.h"
#include "محرك_الفيديو.h"
#include "الخط.h"

void draw_rect(int x, int y, int w, int h, uint32_t color) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            fb_putpixel(x + j, y + i, color);
        }
    }
}
