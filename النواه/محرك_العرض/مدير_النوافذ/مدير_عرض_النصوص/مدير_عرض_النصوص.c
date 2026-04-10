#include "مدير_عرض_النصوص.h"
#include "../محرك_الفيديو/الخط.h"

// رسم حرف 8x8 داخل buffer
void draw_char_buffer(uint32_t* buffer, int bw,
                      int x, int y,
                      char c, uint32_t color) {

    uint8_t* glyph = (uint8_t*) font8x8[(int)c];

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {


            if (glyph[row] & (1 << (7 - col))) {

                int px = x + col;
                int py = y + row;

                buffer[py * bw + px] = color;
            }
        }
    }
}

void draw_string_buffer(uint32_t* buffer, int bw,
                        int x, int y,
                        const char* str, uint32_t color) {

    int cursor_x = x;

    for (int i = 0; str[i] != '\0'; i++) {

        if (str[i] == '\n') {
            y += 10;
            cursor_x = x;
            continue;
        }

        draw_char_buffer(buffer, bw, cursor_x, y, str[i], color);

        cursor_x += 8; // عرض الحرف
    }
}
