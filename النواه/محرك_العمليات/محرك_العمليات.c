#include "محرك_العمليات.h"
#include "../محرك_الذاكرة/محرك_الذاكرة.h"

static int next_pid = 1;

process_t* process_create(void (*entry)()) {

    process_t* p = kmalloc(sizeof(process_t));

    p->pid = next_pid++;
    p->entry = entry;

    p->stack = kmalloc(4096);

    return p;
}

void process_run(process_t* p) {
    p->entry();
}

void process_init() {
    next_pid = 1;
}
