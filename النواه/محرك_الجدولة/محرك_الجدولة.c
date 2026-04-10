#include <stdint.h>
#include "محرك_الجدولة.h"

static process_t* process_list[MAX_PROCESSES];
extern void context_switch(uint64_t* old_rsp, uint64_t new_rsp);
static int process_count = 0;
static int current = 0;
process_t* scheduler_next();


void scheduler_init() {
    process_count = 0;
}

void scheduler_add(process_t* p) {

    if (process_count >= MAX_PROCESSES)
        return;

    process_list[process_count++] = p;
}




static process_t* current_process = 0;

void schedule() {

    process_t* next = scheduler_next();

    if (current_process == 0) {
        current_process = next;
        asm volatile("mov %0, %%rsp" :: "r"(next->rsp));
        return;
    }

    process_t* prev = current_process;
    current_process = next;

    context_switch(&prev->rsp, next->rsp);
}

process_t* scheduler_next() {                         // دالة للتبديل بين المهام :)

    if (process_count == 0) return 0;

    current = (current + 1) % process_count;

    return process_list[current];
}

void scheduler_run() {                                      // الخطوه الواحده افضل من الحلقة الا نهائية :)

    for (int i = 0; i < process_count; i++) {
        process_run(process_list[i]);
    }
}
