#include "../../../../محرك_التعريفات/محرك_التعريفات.h"
#include "../المسارات/المسارات.h"
#include "../المجلدات/المجلدات.h"
#include "../العقدة/العقدة.h"
#include "القراءة.h"

#define FILE_TABLE_SECTOR 2
#define FILE_TABLE_SECTORS 8
#define SECTOR_SIZE 512

extern file_entry_t table[];

void diskfs_read(const char* path, char* buffer)
{

    inode_load();

    for (int s = 0; s < FILE_TABLE_SECTORS; s++) {

        ata_read_sector(
            FILE_TABLE_SECTOR + s,
            ((unsigned char*)table) + (s * SECTOR_SIZE)
        );
    }

    int id = arabfs_resolve(path);

    if (id < 0) {

        buffer[0] = 0;

        return;
    }

    int inode_id = table[id].inode_id;

    inode_t* node = inode_get(inode_id);

    unsigned int size = node->size;

    unsigned int sectors =
        (size + SECTOR_SIZE - 1) / SECTOR_SIZE;


    for (unsigned int i = 0; i < sectors; i++) {

        ata_read_sector(

            node->blocks[i],

            (unsigned char*)buffer + (i * SECTOR_SIZE)

        );
    }
}

const char* diskfs_read_wrapper(const char* path, char* buffer)
{

    diskfs_read(path, buffer);

    return buffer;
}
