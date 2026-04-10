#ifndef VIRTUAL_MOUSE_H
#define VIRTUAL_MOUSE_H

#include <stdint.h>

extern int mouse_x;
extern int mouse_y;

void mouse_init();
void mouse_handler();
void draw_cursor();

#endif
