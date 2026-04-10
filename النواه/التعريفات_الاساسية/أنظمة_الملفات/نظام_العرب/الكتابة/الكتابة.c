#include "../../../../محرك_التعريفات/محرك_التعريفات.h"
#include "../العقدة/العقدة.h"
#include "الكتابة.h"

#define SECTOR_SIZE 512
#define DATA_START_SECTOR 10
#define INODE_DIRECT_BLOCKS 12

static void arabfs_memset(void* ptr, int value, int size)
{
    unsigned char* p = ptr;

    for (int i = 0; i < size; i++)
        p[i] = value;
}

static int strlen(const char* str)
{
    int len = 0;

    while (str[len])
        len++;

    return len;
}

void arabfs_write_data(inode_t* node, const char* data)
{
    unsigned int size = strlen(data);

    node->size = size;

    unsigned int sectors =
        (size + SECTOR_SIZE - 1) / SECTOR_SIZE;

    for (unsigned int i = 0; i < sectors && i < INODE_DIRECT_BLOCKS; i++)
    {
        unsigned int sector =
            DATA_START_SECTOR + (node->used * 20) + i;

        node->blocks[i] = sector;

        unsigned char buffer[SECTOR_SIZE];

        arabfs_memset(buffer, 0, SECTOR_SIZE);

        for (unsigned int j = 0; j < SECTOR_SIZE; j++)
        {
            unsigned int index = (i * SECTOR_SIZE) + j;

            if (index < size)
                buffer[j] = data[index];
        }

        ata_write_sector(sector, buffer);
    }
}
