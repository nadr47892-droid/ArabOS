#ifndef FILE_H
#define FILE_H

#include <stdint.h>

typedef struct {
    char name[128];
    uint8_t* data;
    uint32_t size;
} file_t;

#endif
