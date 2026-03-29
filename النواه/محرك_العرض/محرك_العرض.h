#ifndef DISPLAY_SERVER_H
#define DISPLAY_SERVER_H

#include <stdint.h>

int window_create(int x, int y, int w, int h, uint32_t color);
void window_move(int id, int dx, int dy);
void display_render();

#endif
