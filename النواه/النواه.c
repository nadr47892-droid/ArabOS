#include <stdint.h>
#include "محرك_المقاطعات/مُدير_جدول_المقاطعات.h"
#include "محرك_المقاطعات/مكتبة_واجهة_الإدخال_والإخراج.h"
#include "التعريفات_الاساسية/المؤقت/المؤقت.h"
#include "التعريفات_الاساسية/لوحات_المفاتيح/لوحة_المفاتيح_الافتراضية/لوحة_المفاتيح_الافتراضية.h"
#include "التعريفات_الاساسية/مؤشرات_الفأرة/مؤشر_الفأرة_الافتراضي/مؤشر_الفأرة_الافتراضية.h"
#include "محرك_الفيديو/محرك_الفيديو.h"
#include "محرك_الذاكرة/مدير_الصفحات/مدير_الصفحات.h"
#include "محرك_النصوص/مكتبة_معالجة_النصوص.h"
#include "محرك_الجدولة/محرك_الجدولة.h"
#include "محرك_العرض/محرك_العرض.h"
#include "القلب/الجوهرة.h"
#include "محرك_الذاكرة/محرك_الذاكرة.h"
#include "محرك_التنفيذ/محرك_التنفيذ.h"
#include "مدير_النوافذ/مدير_النوافذ.h"
#include "محرك_التطبيقات/محرك_التطبيقات.h"
#include "محرك_انظمة_الملفات/file.h"

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

extern void isr_timer();
extern void isr_keyboard();
extern void isr_mouse();

/* ================= MULTIBOOT ================= */

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

struct multiboot2_tag_module {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
};

/* ================= الوسيط ================= */



file_t files[16];
int file_count = 0;

file_t* init_file = 0;
file_t* txt_file = 0;

int hex_to_int(char* str, int len) {
    int val = 0;
    for (int i = 0; i < len; i++) {
        val <<= 4;
        char c = str[i];
        if (c >= '0' && c <= '9') val |= (c - '0');
        else if (c >= 'A' && c <= 'F') val |= (c - 'A' + 10);
    }
    return val;
}

void parse_الوسيط(uint8_t* start, uint32_t size) {
    uint8_t* p = start;
    uint8_t* end = start + size;

    while (p < end) {

        // magic 070701
        if (!(p[0]=='0' && p[1]=='7' && p[2]=='0' && p[3]=='7')) break;

        int namesize = hex_to_int((char*)(p + 94), 8);
        int filesize = hex_to_int((char*)(p + 54), 8);

        char* name = (char*)(p + 110);
        uint8_t* data = (uint8_t*)(p + 110 + namesize);

        // محاذاة
        if ((uintptr_t)data % 4)
            data += 4 - ((uintptr_t)data % 4);

        if (name[0] && file_count < 16) {
            file_t* f = &files[file_count++];

            int i = 0;
            while (name[i] && i < 127) {
                f->name[i] = name[i];
                i++;
            }
            f->name[i] = 0;

            f->data = data;
            f->size = filesize;
        }

        // نهاية الأرشيف
        if (name[0]=='T') break;

        p = data + filesize;

        if ((uintptr_t)p % 4)
            p += 4 - ((uintptr_t)p % 4);
    }
}

file_t* find_file(const char* name) {
    for (int i = 0; i < file_count; i++) {
        char* a = files[i].name;
        int j = 0;
        while (name[j] && a[j] && name[j]==a[j]) j++;
        if (name[j]==0 && a[j]==0) return &files[i];
    }
    return 0;
}

/* ================= العتاد ================= */

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


/* ================= طرفية ================= */

int win;

char command[128];
int cmd_len = 0;
char terminal_buffer[4096];

// ===== أدوات =====


int starts_with(const char* str, const char* prefix) {
    int i = 0;
    while (prefix[i]) {
        if (str[i] != prefix[i]) return 0;
        i++;
    }
    return 1;
}

void append_text(const char* str) {

    int len = 0;
    while (terminal_buffer[len]) len++;

    int i = 0;
    while (str[i] && len < 255) {
        terminal_buffer[len++] = str[i++];
    }

    terminal_buffer[len] = 0;
}

void list_callback(const char* name) {
    append_text(name);
    append_text("\n");
}

// ===== أوامر =====

void execute_command(const char* cmd) {

    append_text("> ");
    append_text(cmd);
    append_text("\n");

    // ===== BASIC =====

    if (strcmp(cmd, "clear") == 0) {
        terminal_buffer[0] = 0;
        return;
    }

    if (strcmp(cmd, "help") == 0) {
        append_text("Commands:\n");
        append_text("help clear ls cat echo touch mkdir rm\n");
        append_text("pwd write read\n");
        return;
    }

    if (strcmp(cmd, "ls") == 0) {

    // عرض ملفات الوسيط
        for (int i = 0; i < file_count; i++) {
            append_text(files[i].name);
            append_text("\n");
        }

        return;
    }

    // ===== CAT =====

    if (starts_with(cmd, "cat ")) {
        const char* name = cmd + 4;
        const char* data = sys_fs_read(name);

        if (data) {
            append_text(data);
            append_text("\n");
        } else {
            append_text("File not found\n");
        }
        return;
    }

    // ===== ECHO =====

    if (starts_with(cmd, "echo ")) {
        append_text(cmd + 5);
        append_text("\n");
        return;
    }

    // ===== TOUCH =====

    if (starts_with(cmd, "touch ")) {
        const char* name = cmd + 6;

        if (sys_fs_create(name))
            append_text("File created\n");
        else
            append_text("Error\n");

        return;
    }

    // ===== MKDIR =====

    if (starts_with(cmd, "mkdir ")) {
        const char* name = cmd + 6;

        if (sys_fs_mkdir(name))
            append_text("Directory created\n");
        else
            append_text("Error\n");

        return;
    }

    // ===== RM =====

    if (starts_with(cmd, "rm ")) {
        const char* name = cmd + 3;

        if (sys_fs_delete(name))
            append_text("Deleted\n");
        else
            append_text("Error deleting\n");

        return;
    }

    // ===== WRITE =====

    if (starts_with(cmd, "write ")) {
        // format: write file text
        const char* p = cmd + 6;

        char name[64];
        int i = 0;

        while (*p && *p != ' ') {
            name[i++] = *p++;
        }
        name[i] = 0;

        if (*p == ' ') p++;

        if (sys_fs_write(name, p))
            append_text("Written\n");
        else
            append_text("Write error\n");

        return;
    }

    if (starts_with(cmd, "run ")) {

        const char* name = cmd + 4;

        file_t* f = find_file(name);

        if (f) {
            app_run(f);
            append_text("App started\n");
        } else {
            append_text("File not found\n");
        }

        return;
    }

    // ===== READ =====

    if (starts_with(cmd, "read ")) {
        const char* name = cmd + 5;
        const char* data = sys_fs_read(name);

        if (data) {
            append_text(data);
            append_text("\n");
        } else {
            append_text("Not found\n");
        }

        return;
    }

    // ===== PWD (مؤقت) =====

    if (strcmp(cmd, "pwd") == 0) {
        append_text("/\n"); // لاحقاً نخليها ديناميكية
        return;
    }

    // ===== UNKNOWN =====

    append_text("Unknown command\n");
}
// ===== عرض =====

void update_display() {
    window_t* w = &windows[win];

    char buffer[256];
    int i = 0;

    // فقط النص الثابت
    while (terminal_buffer[i]) {
        buffer[i] = terminal_buffer[i];
        i++;
    }

    // prompt
    buffer[i++] = '>';
    buffer[i++] = ' ';

    // command الحالي
    for (int j = 0; j < cmd_len; j++) {
        buffer[i++] = command[j];
    }

    buffer[i] = 0;

    // عرض
    int k = 0;
    while (buffer[k] && k < 255) {
        w->text[k] = buffer[k];
        k++;
    }
    w->text[k] = 0;
}

// ===== GUI TASK =====

void gui_task() {

    win = window_create(100, 100, 400, 300, 0x334455);
    //window_t* w = &windows[win];

    terminal_buffer[0] = 0;

    append_text("ArabOS Terminal\n");
    append_text("type help\n\n");

    while (1) {

        char c = keyboard_read();

        if (c == '\n') {
            command[cmd_len] = 0;

            execute_command(command);

            cmd_len = 0;
        }
        else if (c == '\b') {
            if (cmd_len > 0) cmd_len--;
        }
        else if (c >= 32 && cmd_len < 127) {
            command[cmd_len++] = c;
        }

        update_display();

        display_render();

        for (volatile int i = 0; i < 100000; i++);
    }
}




typedef struct {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
} tar_header_t;

int oct_to_int(char* str, int len) {
    int val = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == 0) break;
        val = val * 8 + (str[i] - '0');
    }
    return val;
}

file_t* parse_tar_and_find(uint8_t* data, uint32_t size, const char* target) {
    static file_t result;

    uint8_t* p = data;
    uint8_t* end = data + size;

    while (p < end) {
        tar_header_t* h = (tar_header_t*)p;

        if (h->name[0] == 0) break;

        int filesize = oct_to_int(h->size, 12);

        uint8_t* file_data = p + 512;

        // مقارنة الاسم
        int i = 0;
        while (target[i] && h->name[i] && target[i] == h->name[i]) i++;

        if (target[i] == 0 && h->name[i] == 0) {
            result.data = file_data;
            result.size = filesize;

            // نسخ الاسم
            int j = 0;
            while (h->name[j] && j < 127) {
                result.name[j] = h->name[j];
                j++;
            }
            result.name[j] = 0;

            return &result;
        }

        // التحرك للملف التالي (512 alignment)
        int jump = ((filesize + 511) / 512) * 512;
        p += 512 + jump;
    }

    return 0;
}

/* ================= KERNEL ================= */

void kernel_main(uint64_t magic, uint64_t addr) {

    if ((uint32_t)magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        while (1);
    }

    uint8_t* ptr = (uint8_t*)addr;
    uint32_t total_size = *(uint32_t*)ptr;
    ptr += 8;

    framebuffer_info_t fb_info;
    int fb_found = 0;

    uint8_t* الوسيط_start = 0;
    uint32_t الوسيط_size = 0;

    while (ptr < (uint8_t*)addr + total_size) {

        struct multiboot2_tag* tag = (struct multiboot2_tag*)ptr;

        if (tag->type == 0) break;

        // framebuffer
        if (tag->type == 8) {
            struct multiboot2_tag_framebuffer* fb = (void*)tag;

            fb_info.address = fb->framebuffer_addr;
            fb_info.width   = fb->framebuffer_width;
            fb_info.height  = fb->framebuffer_height;
            fb_info.pitch   = fb->framebuffer_pitch;
            fb_info.bpp     = fb->framebuffer_bpp;

            fb_found = 1;
        }

        // الوسيط
        if (tag->type == 3) {
            struct multiboot2_tag_module* mod = (void*)tag;

            الوسيط_start = (uint8_t*)(uintptr_t)mod->mod_start;
            الوسيط_size  = mod->mod_end - mod->mod_start;
        }

        ptr += (tag->size + 7) & ~7;
    }

    if (!fb_found) {
        while (1);
    }

    memory_init();
    paging_init(fb_info.address);
    paging_enable(paging_get_pml4());
    uint64_t frame = alloc_frame();
    map_page(0x400000, frame);
    void* x = kmalloc(100);

    *(char*)x = 42;

    fb_init(&fb_info);
    fb_clear(0x00224488);

    // تحميل الوسيط

    if (الوسيط_start) {
        parse_الوسيط(الوسيط_start, الوسيط_size);

        init_file = find_file("init.txt");

        file_t* rootfs = find_file("rootfs.tar");

        if (rootfs) {
            txt_file = parse_tar_and_find(rootfs->data, rootfs->size, "./txt.txt");
        }
    }

    /* ===== تشغيل النظام ===== */
    update_display();
    display_render();
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
