#include <stdint.h>
#include "محرك_المقاطعات/مُدير_جدول_المقاطعات.h"
#include "محرك_المقاطعات/مكتبة_واجهة_الإدخال_والإخراج.h"
#include "التعريفات_الاساسية/المؤقت/المؤقت.h"
#include "التعريفات_الاساسية/لوحات_المفاتيح/لوحة_المفاتيح_الافتراضية/لوحة_المفاتيح_الافتراضية.h"
#include "التعريفات_الاساسية/مؤشرات_الفأرة/مؤشر_الفأرة_الافتراضي/مؤشر_الفأرة_الافتراضية.h"
#include "محرك_الفيديو/محرك_الفيديو.h"
#include "محرك_الفيديو/عارض_الصورة.h"
#include "محرك_الذاكرة/مدير_الصفحات/مدير_الصفحات.h"
#include "محرك_الجدولة/محرك_الجدولة.h"
#include "محرك_العرض/محرك_العرض.h"
#include "القلب/الجوهرة.h"
#include "مدير_النوافذ/مدير_النوافذ.h"

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

extern void isr_timer();
extern void isr_keyboard();
extern void isr_mouse();
extern volatile char last_key;

struct multiboot2_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot2_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
};

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xF9);
    outb(0xA1, 0xEF);
}

int win;

void gui_task() {
    // إنشاء نافذة بحجم مناسب
    win = window_create(100, 100, 400, 300, 0x334455);
    
    while (1) {
        if (last_key == 0x11) window_move(win, 0, -5);
        if (last_key == 0x1F) window_move(win, 0, 5);
        if (last_key == 0x1E) window_move(win, -5, 0);
        if (last_key == 0x20) window_move(win, 5, 0);
        
        display_render();
        
        for (volatile int i = 0; i < 100000; i++);
    }
}

void kernel_main(uint64_t magic, uint64_t addr) {
    if ((uint32_t)magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        while (1);
    }
    
    uint8_t* ptr = (uint8_t*)addr;
    uint32_t total_size = *(uint32_t*)ptr;
    ptr += 8;
    
    framebuffer_info_t fb_info;
    int fb_found = 0;
    char* cmdline = 0;
    int text_mode = 0;
    
    while (ptr < (uint8_t*)addr + total_size) {
        struct multiboot2_tag* tag = (struct multiboot2_tag*)ptr;
        
        if (tag->type == 0) break;
        
        if (tag->type == 1) {
            cmdline = (char*)(ptr + 8);
        }
        
        if (tag->type == 8) {
            struct multiboot2_tag_framebuffer* fb = (struct multiboot2_tag_framebuffer*)tag;
            fb_info.address = fb->framebuffer_addr;
            fb_info.width   = fb->framebuffer_width;
            fb_info.height  = fb->framebuffer_height;
            fb_info.pitch   = fb->framebuffer_pitch;
            fb_info.bpp     = fb->framebuffer_bpp;
            fb_found = 1;
        }
        
        ptr += (tag->size + 7) & ~7;
    }
    
    if (cmdline) {
        char* p = cmdline;
        while (*p) {
            if (p[0]=='t' && p[1]=='e' && p[2]=='x' && p[3]=='t') {
                text_mode = 1;
                break;
            }
            p++;
        }
    }
    
    if (text_mode) {
        core_init();
        core_run();
        return;
    }
    
    if (fb_found) {
        fb_init(&fb_info);
        fb_clear(0x00224488);
        
        //  تهيئة مدير النوافذ أولاً
        window_manager_init();
        
        pic_remap();
        idt_init();
        
        idt_set_gate(32, (uint64_t)isr_timer);
        idt_set_gate(33, (uint64_t)isr_keyboard);
        idt_set_gate(44, (uint64_t)isr_mouse);
        
        mouse_init();
        keyboard_init();
        timer_init();
        
        asm volatile("sti");
        
        process_t* gui = process_create(gui_task);
        scheduler_add(gui);
        
        while (1) {
            schedule();
        }
    }
    
    while (1);
}
