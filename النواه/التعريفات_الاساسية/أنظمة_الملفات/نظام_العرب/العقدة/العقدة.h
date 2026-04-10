#ifndef ARABFS_INODE_H
#define ARABFS_INODE_H

#define MAX_INODES 65536

/* عدد البلوكات المباشرة */
#define INODE_DIRECT_BLOCKS 12

typedef struct {

    unsigned int used;

    unsigned int size;

    unsigned int type;

    unsigned int parent;

    char name[32];

    /* direct blocks */
    unsigned int blocks[INODE_DIRECT_BLOCKS];

    /* indirect block */
    unsigned int indirect_block;

    /* double indirect block */
    unsigned int double_indirect_block;

} inode_t;

void inode_load();
void inode_save();

int inode_alloc();

inode_t* inode_get(int id);

#endif
