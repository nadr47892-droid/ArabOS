#include "محرك_ربط_العمليات.h"

#define MAX_MESSAGES 32

static Message queue[MAX_MESSAGES];
static int head = 0;
static int tail = 0;

void ipc_init() {
    head = tail = 0;
}

void ipc_send(Message msg) {
    queue[tail] = msg;
    tail = (tail + 1) % MAX_MESSAGES;
}

void ipc_dispatch() {
    if (head == tail)
        return;

    Message msg = queue[head];
    head = (head + 1) % MAX_MESSAGES;

    // حالياً لا يوجد توجيه حقيقي (نسخة تعليمية)
}

