#include "محرك_الفيديو.h"

static uint32_t* fb;
static uint32_t fb_width;
static uint32_t fb_height;
static uint32_t fb_pitch;

void fb_init(framebuffer_info_t* fb_info) {
    fb = (uint32_t*) fb_info->address;
    fb_width = fb_info->width;
    fb_height = fb_info->height;
    fb_pitch = fb_info->pitch / 4;
}

void fb_putpixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 ||
        (uint32_t)x >= fb_width ||
        (uint32_t)y >= fb_height)
        return;

    fb[y * fb_pitch + x] = color;
}
void fb_clear(uint32_t color) {
    for (uint32_t y = 0; y < fb_height; y++) {
        for (uint32_t x = 0; x < fb_width; x++) {
            fb[y * fb_pitch + x] = color;
        }
    }
}

uint32_t fb_get_width() { return fb_width; }
uint32_t fb_get_height() { return fb_height; }
