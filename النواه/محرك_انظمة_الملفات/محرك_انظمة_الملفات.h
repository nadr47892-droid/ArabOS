#ifndef FS_ENGINE_H
#define FS_ENGINE_H

// تعريف بنية نظام الملفات (الواجهة)
typedef struct {
    const char* name;
    int (*exists)(const char* path);
    int (*create)(const char* path, const char* data);
    int (*mkdir)(const char* path);
    int (*write)(const char* path, const char* data);
    const char* (*read)(const char* path, char* buffer);
    void (*list)(const char* path, void (*callback)(const char*));
    int (*delete_file)(const char* path);
    int (*delete_dir)(const char* path);
    int (*copy)(const char* src, const char* dest);  // دالة النسخ الجديدة
} fs_driver_t;

// وظائف المحرك كطبقة تجريد
void fs_engine_init();
void fs_engine_register_driver(fs_driver_t* driver);

// هذه الدوال ستوجه الطلبات للتعريف (Driver) المسجل
int fs_engine_exists(const char* path);
int fs_engine_create(const char* path, const char* data);
int fs_engine_mkdir(const char* path);
int fs_engine_write(const char* path, const char* data);
const char* fs_engine_read(const char* path, char* buffer);
void fs_engine_list(const char* path, void (*callback)(const char*));
int fs_engine_delete(const char* path);
int fs_engine_delete_dir(const char* path);
int fs_engine_copy(const char* src, const char* dest);  // دالة النسخ الجديدة

#endif
