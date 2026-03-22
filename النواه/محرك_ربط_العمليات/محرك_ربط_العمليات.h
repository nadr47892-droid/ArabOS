#ifndef IPC_H
#define IPC_H

typedef struct {
    int sender;
    int receiver;
    int type;
    void* data;
} Message;

void ipc_init();
void ipc_send(Message msg);
void ipc_dispatch();

#endif

