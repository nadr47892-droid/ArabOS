#include "../../../محرك_التعريفات/محرك_التعريفات.h"
#include "../التعريف_العام/التعريف_العام.h"
#include "../المسارات/المسارات.h"
#include "../العقدة/العقدة.h"
#include "../الكتابة/الكتابة.h"
#include "../نظام_العرب.h"
#include "الملفات.h"
#define SECTOR_SIZE 512
#define FILE_TABLE_SECTOR 2
#define FILE_TABLE_SECTORS 8

extern file_entry_t table[];

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

// في ملف الملفات.c، تأكد من وجود هذه الدالة
int diskfs_create(const char* path, const char* data)
{
    inode_load();

    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {

        ata_read_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }

    unsigned int parent = 0;

    char parent_path[64];
    char name[32];

    arabfs_get_parent(path, parent_path, name);

    if (parent_path[0])
        parent = arabfs_resolve(parent_path);

    if ((int)parent < 0)
        return 0;

    int inode_id = inode_alloc();

    if (inode_id < 0)
        return 0;

    inode_t* node = inode_get(inode_id);

    node->type = 0;
    node->parent = parent;

    strcpy(node->name, name);

    arabfs_write_data(node, data);

    inode_save();

    for (int i = 0; i < MAX_FILES; i++) {

        if (!table[i].used) {

            table[i].used = 1;
            table[i].type = 0;
            table[i].inode_id = inode_id;
            table[i].parent_id = parent;

            strcpy(table[i].name, name);

            break;
        }
    }

    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {

        ata_write_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }

    return 1;
}

int diskfs_exists_wrapper(const char* path)
{
    return arabfs_resolve(path) >= 0;
}

int diskfs_delete(const char* path) {
    // قراءة جدول الملفات
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {
        ata_read_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }
    
    // الحصول على ID الملف
    int id = arabfs_resolve(path);
    if (id < 0)
        return 0;
    
    // لا يمكن حذف المجلدات بهذه الدالة
    if (table[id].type != 0)
        return 0;
    
    // تحرير inode
    int inode_id = table[id].inode_id;
    inode_t* node = inode_get(inode_id);
    
    // تحرير البلوكات
    unsigned int sectors = (node->size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    for (unsigned int i = 0; i < sectors && i < INODE_DIRECT_BLOCKS; i++) {
        if (node->blocks[i] != 0) {
            // يمكن إضافة دالة لتحرير البلوكات هنا
            node->blocks[i] = 0;
        }
    }
    
    // تعطيل inode
    node->used = 0;
    node->size = 0;
    
    // حفظ inode
    inode_save();
    
    // إزالة من جدول الملفات
    table[id].used = 0;
    
    // حفظ جدول الملفات
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {
        ata_write_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }
    
    return 1;
}
