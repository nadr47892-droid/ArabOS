#ifndef ARABFS_BLOCK_H
#define ARABFS_BLOCK_H

#define BLOCK_BITMAP_SECTOR 20
#define BLOCK_BITMAP_SIZE 4096

void block_bitmap_load();
void block_bitmap_save();

int block_alloc();
void block_free(int block);

#endif
