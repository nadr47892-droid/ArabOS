#include "../../../../محرك_التعريفات/محرك_التعريفات.h"
#include "العقدة.h"

#define INODE_TABLE_SECTOR 20
#define INODE_TABLE_SECTORS 128
#define SECTOR_SIZE 512

inode_t inode_table[MAX_INODES];

static void memzero(void* ptr, int size)
{
    unsigned char* p = ptr;

    for (int i = 0; i < size; i++)
        p[i] = 0;
}

/* تحميل جدول العقد من القرص */

void inode_load()
{

    for (int s = 0; s < INODE_TABLE_SECTORS; s++)
    {

        ata_read_sector(
            INODE_TABLE_SECTOR + s,
            ((unsigned char*)inode_table) + (s * SECTOR_SIZE)
        );
    }
}

/* حفظ جدول العقد */

void inode_save()
{

    for (int s = 0; s < INODE_TABLE_SECTORS; s++)
    {

        ata_write_sector(
            INODE_TABLE_SECTOR + s,
            ((unsigned char*)inode_table) + (s * SECTOR_SIZE)
        );
    }
}

/* حجز inode جديد */

int inode_alloc()
{

    for (int i = 0; i < MAX_INODES; i++)
    {

        if (!inode_table[i].used)
        {

            inode_table[i].used = 1;

            for (int j = 0; j < INODE_DIRECT_BLOCKS; j++)
                inode_table[i].blocks[j] = 0;

            return i;
        }
    }

    return -1;
}

/* الحصول على inode */

inode_t* inode_get(int id)
{

    if (id < 0 || id >= MAX_INODES)
        return 0;

    return &inode_table[id];
}
