#include <stdint.h>
#include <المؤقت.h>
#include "محرك_الجدولة/محرك_الجدولة.h"
#include <../../../محرك_المقاطعات/مكتبة_واجهة_الإدخال_والإخراج.h>
#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40


volatile int ticks = 0;

void timer_init() {
    uint32_t freq = 100;
    uint32_t divisor = 1193180 / freq;

    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}


extern process_t* scheduler_next();

void timer_handler() {
    ticks++;

    outb(0x20, 0x20);
}
