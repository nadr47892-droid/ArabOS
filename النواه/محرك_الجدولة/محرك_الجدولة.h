#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../محرك_العمليات/محرك_العمليات.h"

#define MAX_PROCESSES 32

void scheduler_init();
void scheduler_add(process_t* p);
void scheduler_run();
void schedule();

#endif
