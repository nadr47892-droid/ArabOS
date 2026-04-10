#include "محرك_التنفيذ.h"
#include "المسارات.h"
#include "../التعريفات_الاساسية/أنظمة_الملفات/نظام_العرب/الملفات/الملفات.h"
#include "التعريفات_الاساسية/أنظمة_الملفات/نظام_العرب/نظام_العرب.h"
#include "../التعريفات_الاساسية/أنظمة_الملفات/نظام_العرب/المجلدات/المجلدات.h"
#include "../التعريفات_الاساسية/لوحات_المفاتيح/لوحة_المفاتيح_الافتراضية/لوحة_المفاتيح_الافتراضية.h"
#include "محرك_التعريفات/محرك_التعريفات.h"

void sys_write(const char* str) {
    console_write(str);
}

char sys_read() {
    return keyboard_getchar();
}

void sys_clear() {
    console_clear();
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outw(unsigned short port, unsigned short val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

void sys_restart() {
    console_write("Restarting...\n");                 // رسالة تظهر قبل أعادة التشغيل للتأكد بأن امر أعادة التشغيل يعمل جيدا
    outb(0x64, 0xFE);
    while (1);
}

void sys_shutdown() {
    console_write("Shutting down...\n");              // رسالة تظهر قبل ايقاف التشغيل للتأكد بأن امر ايقاف التشغيل يعمل جيدا
    outw(0x604, 0x2000);   // QEMU poweroff
    while (1);
}

#include "../محرك_انظمة_الملفات/محرك_انظمة_الملفات.h"

int sys_fs_create(const char* name) {
    diskfs_create(name, "");
    return 1;
}

int sys_fs_write(const char* name, const char* data) {
    diskfs_create(name, data);
    return 1;
}

static char read_buffer[512];

const char* sys_fs_read(const char* name) {
    diskfs_read(name, read_buffer);
    return read_buffer;
}

static void list_callback(const char* name) {
    console_write(name);
    console_write("\n");
}

void sys_fs_list(const char* path,
                 void (*callback)(const char*)) {

    (void)path;
    unsigned int id = 0;

    if (path && path[0] != 0)
        id = arabfs_resolve(path);

    arabfs_list_path(id, callback);
}

int sys_fs_mkdir(const char* path) {
    return arabfs_mkdir(path);
}

int sys_fs_delete(const char* path) {
    return fs_engine_delete(path);
}

int sys_fs_exists(const char* path) {
    return fs_engine_exists(path);
}

int sys_fs_copy(const char* src, const char* dest) {
    return fs_engine_copy(src, dest);
}
