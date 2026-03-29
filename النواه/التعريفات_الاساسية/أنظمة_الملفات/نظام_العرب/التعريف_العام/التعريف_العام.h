#ifndef ARABFS_TYPES_H
#define ARABFS_TYPES_H

#define MAX_FILES 65536

typedef struct {

    unsigned int used;
    unsigned int type;        // 0 ملف 1 مجلد

    unsigned int id;
    unsigned int parent_id;

    unsigned int inode_id;    // رابط الـ inode

    char name[32];

} file_entry_t;


/* جدول الملفات */
extern file_entry_t table[MAX_FILES];

#endif
