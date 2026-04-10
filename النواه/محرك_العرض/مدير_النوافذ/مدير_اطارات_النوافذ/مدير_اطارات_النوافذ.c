#include "مدير_اطارات_النوافذ.h"
#include "../محرك_الفيديو/عارض_الصورة.h"

#define FRAME_COLOR   0x222222
#define TITLE_COLOR   0x4444AA
#define CLOSE_COLOR   0xFF0000
#define MAXIMIZE_COLOR 0x00FF00

void window_draw_frame(window_t* win) {
    if (!win->buffer) return;
    if (win->w < 30 || win->h < 30) return;
    
    // شريط العنوان
    for (int y = 0; y < 18 && y < win->h; y++) {
        for (int x = 0; x < win->w; x++) {
            win->buffer[y * win->w + x] = TITLE_COLOR;
        }
    }
    
    // زر الإغلاق
    int close_x = win->w - 14;
    int close_y = 2;
    if (close_x >= 0 && close_x + 12 < win->w && close_y + 12 < win->h) {
        for (int y = close_y; y < close_y + 12; y++) {
            for (int x = close_x; x < close_x + 12; x++) {
                win->buffer[y * win->w + x] = CLOSE_COLOR;
            }
        }
    }
    
    // زر التكبير
    int max_x = win->w - 28;
    int max_y = 2;
    if (max_x >= 0 && max_x + 12 < win->w && max_y + 12 < win->h) {
        for (int y = max_y; y < max_y + 12; y++) {
            for (int x = max_x; x < max_x + 12; x++) {
                win->buffer[y * win->w + x] = MAXIMIZE_COLOR;
            }
        }
    }
    
    // الحدود
    for (int x = 0; x < win->w; x++) {
        win->buffer[x] = FRAME_COLOR;  // أعلى
        win->buffer[(win->h - 1) * win->w + x] = FRAME_COLOR;  // أسفل
    }
    
    for (int y = 0; y < win->h; y++) {
        win->buffer[y * win->w] = FRAME_COLOR;  // يسار
        win->buffer[y * win->w + (win->w - 1)] = FRAME_COLOR;  // يمين
    }
    
    // منطقة resize
    int resize_size = 10;
    for (int y = win->h - resize_size; y < win->h; y++) {
        for (int x = win->w - resize_size; x < win->w; x++) {
            if (x >= 0 && y >= 0 && x < win->w && y < win->h) {
                win->buffer[y * win->w + x] = 0xAAAAAA;
            }
        }
    }
}
