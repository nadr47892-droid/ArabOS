#include <stdint.h>

#ifndef PROCESS_H
#define PROCESS_H

typedef struct {

    int pid;
    void (*entry)();
    uint64_t rsp;

    void* stack;

} process_t;

void process_init();
process_t* process_create(void (*entry)());
void process_run(process_t* p);

#endif
