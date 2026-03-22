#include "محرك_التعريفات/محرك_التعريفات.h"
#include "../المسارات/المسارات.h"
#include "المجلدات.h"

#define FILE_TABLE_SECTOR 2
#define FILE_TABLE_SECTORS 8
#define SECTOR_SIZE 512

extern file_entry_t table[MAX_FILES];


/* ================= أدوات داخلية ================= */

static int strcmp(const char* a, const char* b)
{
    int i = 0;

    while (a[i] && b[i]) {
        if (a[i] != b[i])
            return 1;
        i++;
    }

    return a[i] != b[i];
}

static void strcpy(char* dest, const char* src)
{
    int i = 0;

    while (src[i]) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = 0;
}


/* ================= البحث عن ملف ================= */

int arabfs_find_entry(const char* name, unsigned int parent)
{
    for (int i = 0; i < MAX_FILES; i++) {

        if (table[i].used &&
            table[i].parent_id == parent &&
            !strcmp(table[i].name, name))
        {
            return i;
        }
    }

    return -1;
}

/* ================= إنشاء مجلد ================= */

int arabfs_mkdir(const char* path)
{
    /* قراءة جدول الملفات */
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {

        ata_read_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }

    unsigned int parent = 0;
    char name[32];

    int i;
    int last = -1;

    for (i = 0; path[i]; i++) {

        if (path[i] == '/')
            last = i;
    }

    if (last >= 0) {

        char parent_path[64];

        for (i = 0; i < last; i++)
            parent_path[i] = path[i];

        parent_path[last] = 0;

        parent = arabfs_resolve(parent_path);

        strcpy(name, path + last + 1);
    }
    else {

        strcpy(name, path);
    }

    if ((int)parent < 0)
        return 0;

    if (arabfs_find_entry(name, parent) >= 0)
        return 0;


    /* إنشاء المجلد */
    for (i = 0; i < MAX_FILES; i++) {

        if (!table[i].used) {

            table[i].used = 1;
            table[i].type = 1;
            table[i].id = i;
            table[i].parent_id = parent;

            strcpy(table[i].name, name);

            /* حفظ الجدول بالكامل */
            for (int s = 0; s < FILE_TABLE_SECTORS; s++) {

                ata_write_sector(
                    FILE_TABLE_SECTOR + s,
                    ((unsigned char*)table) + (s * SECTOR_SIZE)
                );
            }

            return 1;
        }
    }

    return 0;
}

/* ================= حذف مجلد ================= */

int arabfs_rmdir(const char* path) {
    // قراءة جدول الملفات
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {
        ata_read_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }
    
    // الحصول على ID المجلد
    int id = arabfs_resolve(path);
    if (id < 0)
        return 0;
    
    // تأكد أنه مجلد (نحول id إلى unsigned int للمقارنة)
    unsigned int uid = (unsigned int)id;
    if (table[uid].type != 1)
        return 0;
    
    // تأكد أن المجلد فارغ
    for (int i = 0; i < MAX_FILES; i++) {
        if (table[i].used && table[i].parent_id == uid) {
            return 0;  // المجلد ليس فارغاً
        }
    }
    
    // لا يمكن حذف المجلد الجذر
    if (uid == 0)
        return 0;
    
    // إزالة من جدول الملفات
    table[uid].used = 0;
    
    // حفظ جدول الملفات
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {
        ata_write_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }
    
    return 1;
}

/* ================= عرض محتويات مجلد ================= */

void arabfs_list_path(unsigned int parent,
                      void (*callback)(const char*))
{
    /* قراءة جدول الملفات */
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {

        ata_read_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }

    for (int i = 0; i < MAX_FILES; i++) {

        if (table[i].used &&
            table[i].parent_id == parent)
        {
            callback(table[i].name);
        }
    }
}
