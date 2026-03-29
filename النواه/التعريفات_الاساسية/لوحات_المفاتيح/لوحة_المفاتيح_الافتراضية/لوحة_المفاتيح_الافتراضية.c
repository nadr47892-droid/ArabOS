#include "لوحة_المفاتيح_الافتراضية.h"
#include "محرك_المقاطعات/مكتبة_واجهة_الإدخال_والإخراج.h"
volatile char last_key = 0;


static int shift_pressed = 0;

static const char keymap[128] = {
  0,27,'1','2','3','4','5','6','7','8',
 '9','0','-','=', '\b',
 '\t',
 'q','w','e','r','t','y','u','i','o','p',
 '[',']','\n',
 0,
 'a','s','d','f','g','h','j','k','l',
 ';','\'','`',
 0,
 '\\',
 'z','x','c','v','b','n','m',
 ',','.','/',
 0,'*',0,' '
};

static const char keymap_shift[128] = {
  0,27,'!','@','#','$','%','^','&','*',
 '(',')','_','+', '\b',
 '\t',
 'Q','W','E','R','T','Y','U','I','O','P',
 '{','}','\n',
 0,
 'A','S','D','F','G','H','J','K','L',
 ':','"','~',
 0,
 '|',
 'Z','X','C','V','B','N','M',
 '<','>','?',
 0,'*',0,' '
};

char keyboard_getchar() {

    while (!(inb(0x64) & 1));

    unsigned char scancode = inb(0x60);

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }

    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return 0;
    }

    if (scancode & 0x80)
        return 0;

    if (scancode >= 128)
        return 0;

    if (shift_pressed)
        return keymap_shift[scancode];

    return keymap[scancode];
}

void keyboard_init() {
    shift_pressed = 0;
}



void keyboard_handler() {

    unsigned char scancode = inb(0x60);

    last_key = scancode;   // 🔥 خزن الرقم مباشرة

    outb(0x20, 0x20);
}
