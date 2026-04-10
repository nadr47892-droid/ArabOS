#include "../../../../محرك_التعريفات/محرك_التعريفات.h"
#include "الكتلة.h"

#define SECTOR_SIZE 512

static unsigned char bitmap[BLOCK_BITMAP_SIZE];

void block_bitmap_load()
{
    for(int i=0;i<8;i++)
        ata_read_sector(BLOCK_BITMAP_SECTOR+i, bitmap + (i*SECTOR_SIZE));
}

void block_bitmap_save()
{
    for(int i=0;i<8;i++)
        ata_write_sector(BLOCK_BITMAP_SECTOR+i, bitmap + (i*SECTOR_SIZE));
}

int block_alloc()
{
    for(int i=0;i<BLOCK_BITMAP_SIZE;i++)
    {
        if(bitmap[i]==0)
        {
            bitmap[i]=1;
            block_bitmap_save();
            return i;
        }
    }

    return -1;
}

void block_free(int block)
{
    bitmap[block]=0;
}
