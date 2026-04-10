#include "محرك_الفيديو.h"
#include "../محرك_الذاكرة/مدير_الكومة/مدير_الكومة.h"
#include <stddef.h>

static uint32_t* fb;           // framebuffer (الشاشة)
static uint32_t* back_buffer;  // back buffer

static uint32_t fb_width;
static uint32_t fb_height;
static uint32_t fb_pitch;
static uint32_t buffer_size;

// ✅ getter
uint32_t* fb_get_back_buffer() {
    return back_buffer;
}

// نسخ سريع
static void fast_copy(uint32_t* dest, uint32_t* src, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        dest[i] = src[i];
    }
}

void fb_init(framebuffer_info_t* fb_info) {

    fb = (uint32_t*) fb_info->address;

    fb_width  = fb_info->width;
    fb_height = fb_info->height;
    fb_pitch  = fb_info->pitch / 4;

    buffer_size = fb_pitch * fb_height;

    // 🔥 تخصيص back buffer
    back_buffer = kmalloc(buffer_size * sizeof(uint32_t));

    if (!back_buffer) {
        while (1); // فشل قاتل
    }

    // 🧹 تنظيف أولي
    for (uint32_t i = 0; i < buffer_size; i++) {
        back_buffer[i] = 0x00000000;
    }
}

// رسم pixel (يكتب في back buffer فقط)
void fb_putpixel(int x, int y, uint32_t color) {

    if (x < 0 || y < 0) return;
    if ((uint32_t)x >= fb_width) return;
    if ((uint32_t)y >= fb_height) return;

    back_buffer[y * fb_pitch + x] = color;
}

// مسح الشاشة
void fb_clear(uint32_t color) {

    for (uint32_t i = 0; i < buffer_size; i++) {
        back_buffer[i] = color;
    }
}

// swap → نقل إلى الشاشة
void fb_swap_buffers() {
    fast_copy(fb, back_buffer, buffer_size);
}

// getters
uint32_t fb_get_width()  { return fb_width; }
uint32_t fb_get_height() { return fb_height; }
uint32_t fb_get_pitch()  { return fb_pitch; }
uint32_t fb_get_buffer_size() { return buffer_size; }
