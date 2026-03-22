#include "محرك_التعريفات.h"

#include "التعريفات_الاساسية/الشاشة/شاشة_VGA_النصية/شاشة_VGA_النصية.h"

/* ================= Console Wrapper ================= */

void console_write(const char* str) {
    vga_write(str);
}

void console_clear() {
    vga_clear();
}

/* ================= INIT ================= */

void driver_init() {
    vga_init();
}
