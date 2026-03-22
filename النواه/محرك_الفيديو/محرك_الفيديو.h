#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
    uint64_t address;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t  bpp;
} framebuffer_info_t;

void fb_init(framebuffer_info_t* fb_info);
void fb_putpixel(int x, int y, uint32_t color);
void fb_clear(uint32_t color);

uint32_t fb_get_width();
uint32_t fb_get_height();

#endif
