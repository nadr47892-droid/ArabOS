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

// ===== رسم حرف =====
void draw_char(int x, int y, char c, uint32_t color) {

    for (int row = 0; row < 8; row++) {
        uint8_t line = font8x8[(int)c][row];

        for (int col = 0; col < 8; col++) {
            if (line & (1 << col)) {
                fb_putpixel(x + col, y + row, color);
            }
        }
    }
}

// ===== رسم نص =====
void draw_string(int x, int y, const char* str, uint32_t color) {

    int i = 0;

    while (str[i]) {
        draw_char(x + i * 8, y, str[i], color);
        i++;
    }
}
