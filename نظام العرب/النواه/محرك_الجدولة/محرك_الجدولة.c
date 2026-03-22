#include "محرك_الجدولة.h"

static process_t* process_list[MAX_PROCESSES];
static int process_count = 0;

void scheduler_init() {
    process_count = 0;
}

void scheduler_add(process_t* p) {

    if (process_count >= MAX_PROCESSES)
        return;

    process_list[process_count++] = p;
}

void scheduler_run() {

    while (1) {

        for (int i = 0; i < process_count; i++) {

            process_run(process_list[i]);

        }

    }

}
