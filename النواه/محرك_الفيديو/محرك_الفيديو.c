#include "محرك_الفيديو.h"
#include <stddef.h>  // إضافة هذا السطر لتعريف NULL

static uint32_t* fb;           // البفر الأساسي (الذي يظهر على الشاشة)
static uint32_t* back_buffer;  // البفر الخلفي (للرسم)
static uint32_t fb_width;
static uint32_t fb_height;
static uint32_t fb_pitch;
static uint32_t buffer_size;   // حجم البفر بالبكسل

// بفر ثابت بحجم كافٍ لمعظم الشاشات (1920x1080)
static uint32_t static_buffer[1920 * 1080];

// دالة نسخ يدوية بدلاً من memcpy
static void fast_copy(uint32_t* dest, uint32_t* src, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        dest[i] = src[i];
    }
}

void fb_init(framebuffer_info_t* fb_info) {
    fb = (uint32_t*) fb_info->address;
    fb_width = fb_info->width;
    fb_height = fb_info->height;
    fb_pitch = fb_info->pitch / 4;
    buffer_size = fb_height * fb_pitch;
    
    // استخدام البفر الثابت دائماً
    back_buffer = static_buffer;
}

void fb_putpixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 ||
        (uint32_t)x >= fb_width ||
        (uint32_t)y >= fb_height)
        return;

    back_buffer[y * fb_pitch + x] = color;
}

void fb_clear(uint32_t color) {
    for (uint32_t y = 0; y < fb_height; y++) {
        for (uint32_t x = 0; x < fb_width; x++) {
            back_buffer[y * fb_pitch + x] = color;
        }
    }
}

void fb_swap_buffers() {
    fast_copy(fb, back_buffer, buffer_size);
}

uint32_t fb_get_width() { return fb_width; }
uint32_t fb_get_height() { return fb_height; }
uint32_t fb_get_pitch() { return fb_pitch; }
uint32_t fb_get_buffer_size() { return buffer_size; }
