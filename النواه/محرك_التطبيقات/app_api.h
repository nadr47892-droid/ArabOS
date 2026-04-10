#ifndef APP_API_H
#define APP_API_H

#include <stdint.h>

typedef struct {
    void (*draw_rect)(int,int,int,int,uint32_t);
    void (*draw_string)(int,int,const char*,uint32_t);
} app_api_t;

#endif
