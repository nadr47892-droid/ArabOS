#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <stdint.h>

// رسم حرف داخل buffer
void draw_char_buffer(uint32_t* buffer, int bw,
                      int x, int y,
                      char c, uint32_t color);

// رسم نص داخل buffer
void draw_string_buffer(uint32_t* buffer, int bw,
                        int x, int y,
                        const char* str, uint32_t color);

#endif
