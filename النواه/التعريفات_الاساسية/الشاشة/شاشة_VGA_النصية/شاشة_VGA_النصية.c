#include "شاشة_VGA_النصية.h"

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static int row = 0;
static int col = 0;
static unsigned short* vga = (unsigned short*) VGA_ADDRESS;

void vga_putc(char c) {

    if (c == '\n') {
        row++;
        col = 0;
        return;
    }

    if (c == '\b') {
        if (col > 0) {
            col--;
            int index = row * VGA_WIDTH + col;
            vga[index] = ' ' | 0x0700;
        }
        return;
    }

    int index = row * VGA_WIDTH + col;
    vga[index] = (unsigned short)c | 0x0700;

    col++;
    if (col >= VGA_WIDTH) {
        col = 0;
        row++;
    }

    if (row >= VGA_HEIGHT) {
        row = 0; // لاحقاً سنضيف scrolling
    }
}

void vga_write(const char* str) {
    for (int i = 0; str[i] != 0; i++)
        vga_putc(str[i]);
}

void vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        vga[i] = ' ' | 0x0700;

    row = 0;
    col = 0;
}

void vga_init() {
    vga_clear();
}
