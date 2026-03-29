#include "محرك_انظمة_الملفات.h"

// حالياً سنفترض وجود نظام ملفات واحد نشط (يمكنك توسيعها لاحقاً لدعم Mount Points)
static fs_driver_t* active_driver = 0;

void fs_engine_init() {
    active_driver = 0;
}

void fs_engine_register_driver(fs_driver_t* driver) {
    active_driver = driver;
}

int fs_engine_exists(const char* path) {
    if (active_driver && active_driver->exists)
        return active_driver->exists(path);
    return 0;
}

int fs_engine_create(const char* path, const char* data) {
    if (active_driver && active_driver->create)
        return active_driver->create(path, data);
    return 0;
}

int fs_engine_mkdir(const char* path) {
    if (active_driver && active_driver->mkdir)
        return active_driver->mkdir(path);
    return 0;
}

int fs_engine_write(const char* path, const char* data) {
    if (active_driver && active_driver->write)
        return active_driver->write(path, data);
    return 0;
}

const char* fs_engine_read(const char* path, char* buffer) {
    if (active_driver && active_driver->read)
        return active_driver->read(path, buffer);
    return 0;
}

void fs_engine_list(const char* path, void (*callback)(const char*)) {
    if (active_driver && active_driver->list)
        active_driver->list(path, callback);
}

int fs_engine_delete(const char* path) {
    if (active_driver && active_driver->delete_file)
        return active_driver->delete_file(path);
    return 0;
}

int fs_engine_delete_dir(const char* path) {
    if (active_driver && active_driver->delete_dir)
        return active_driver->delete_dir(path);
    return 0;
}

int fs_engine_copy(const char* src, const char* dest) {
    if (active_driver && active_driver->copy)
        return active_driver->copy(src, dest);
    return 0;
}
