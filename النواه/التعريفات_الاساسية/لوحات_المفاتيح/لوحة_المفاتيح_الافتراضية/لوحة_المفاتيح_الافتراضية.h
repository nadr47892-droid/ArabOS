#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_init();
char keyboard_getchar();
extern volatile char last_key;
void keyboard_handler();
char keyboard_read();

#endif
