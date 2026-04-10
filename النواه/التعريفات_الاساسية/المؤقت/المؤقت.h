#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
extern volatile int ticks;


void timer_init();
void timer_handler();

#endif
