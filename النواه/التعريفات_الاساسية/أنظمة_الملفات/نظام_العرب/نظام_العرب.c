#include "../../../محرك_انظمة_الملفات/محرك_انظمة_الملفات.h"
#include "../../../محرك_التعريفات/محرك_التعريفات.h"
#include "المسارات/المسارات.h"
#include "المجلدات/المجلدات.h"
#include "الملفات/الملفات.h"
#include "التعريف_العام/التعريف_العام.h"
#include "العقدة/العقدة.h"
#include "الكتابة/الكتابة.h"
#include "القراءة/القراءة.h"
#include "نظام_العرب.h"
#include "التهيئة.h"


#define SECTOR_SIZE 512
#define FILE_TABLE_SECTOR 2
#define FILE_TABLE_SECTORS 8
#define DATA_START_SECTOR 10

// تعريف الثوابت المطلوبة
#ifndef INODE_DIRECT_BLOCKS
#define INODE_DIRECT_BLOCKS 12
#endif

// الوصول إلى الجدول العام
extern file_entry_t table[MAX_FILES];

static int arabfs_delete(const char* path) {
    char delete_path[128];
    int i;
    
    // نسخ المسار
    for (i = 0; path[i] && i < 127; i++) {
        delete_path[i] = path[i];
    }
    delete_path[i] = 0;
    
    // قراءة جدول الملفات
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {
        ata_read_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }
    
    // الحصول على ID العنصر
    int id = arabfs_resolve(delete_path);
    if (id < 0) {
        return 0;
    }
    
    unsigned int uid = (unsigned int)id;
    
    // التحقق من وجود العنصر
    if (!table[uid].used) {
        return 0;
    }
    
    // إذا كان مجلداً
    if (table[uid].type == 1) {
        // تأكد أن المجلد فارغ
        for (i = 0; i < MAX_FILES; i++) {
            if (table[i].used && table[i].parent_id == uid) {
                return 0;  // المجلد ليس فارغاً
            }
        }
        
        // لا يمكن حذف المجلد الجذر
        if (uid == 0) {
            return 0;
        }
        
        // إزالة من جدول الملفات
        table[uid].used = 0;
    }
    else {
        // إذا كان ملفاً
        int inode_id = table[uid].inode_id;
        
        // تحميل جدول inodes
        inode_load();
        
        inode_t* node = inode_get(inode_id);
        if (!node || !node->used) {
            return 0;
        }
        
        // تحرير البلوكات (مسح البيانات)
        unsigned int sectors = (node->size + SECTOR_SIZE - 1) / SECTOR_SIZE;
        unsigned char zero_buffer[SECTOR_SIZE];
        
        // تعبئة buffer بالأصفار
        for (i = 0; i < SECTOR_SIZE; i++) {
            zero_buffer[i] = 0;
        }
        
        for (unsigned int bi = 0; bi < sectors && bi < INODE_DIRECT_BLOCKS; bi++) {
            if (node->blocks[bi] != 0) {
                // مسح القطاع بكتابة أصفار
                ata_write_sector(node->blocks[bi], zero_buffer);
                node->blocks[bi] = 0;
            }
        }
        
        // تعطيل inode
        node->used = 0;
        node->size = 0;
        
        // حفظ inode
        inode_save();
        
        // إزالة من جدول الملفات
        table[uid].used = 0;
    }
    
    // حفظ جدول الملفات
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {
        ata_write_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }
    
    return 1;
}

// دالة نسخ الملفات - نسخة مصححة
static int arabfs_copy(const char* src, const char* dest) {
    char buffer[512];  // مخزن مؤقت للقراءة
    char temp_dest[256];
    int i;
    
    // نسخ المسار الهدف
    for (i = 0; dest[i] && i < 255; i++) {
        temp_dest[i] = dest[i];
    }
    temp_dest[i] = 0;
    
    // قراءة جدول الملفات
    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {
        ata_read_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }
    
    // التحقق من وجود المصدر باستخدام الدالة المباشرة
    int src_id = arabfs_resolve(src);
    if (src_id < 0) {
        return 0;  // المصدر غير موجود
    }
    
    // التحقق من أن المصدر ليس مجلداً (نسخ الملفات فقط حالياً)
    unsigned int src_uid = (unsigned int)src_id;
    if (table[src_uid].type != 0) {
        return 0;  // المصدر مجلد - نسخ المجلدات غير مدعوم حالياً
    }
    
    // قراءة محتوى المصدر مباشرة
    diskfs_read(src, buffer);
    
    // إنشاء الملف الهدف بنفس المحتوى
    return diskfs_create(temp_dest, buffer);
}

static fs_driver_t arabfs_driver = {
    .name = "ArabFS",
    .exists = diskfs_exists_wrapper,
    .create = diskfs_create,
    .mkdir = arabfs_mkdir,
    .write = 0,
    .read = diskfs_read_wrapper,
    .list = 0,
    .delete_file = arabfs_delete,
    .delete_dir = arabfs_delete,
    .copy = arabfs_copy  // إضافة دالة النسخ
};

fs_driver_t* get_arabfs_driver()
{
    return &arabfs_driver;
}
