#ifndef VGA_TEXT_H
#define VGA_TEXT_H

void vga_init();
void vga_clear();
void vga_write(const char* str);
void vga_putc(char c);

#endif
