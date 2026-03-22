#ifndef DISK_FS_H
#define DISK_FS_H

#include "../../../محرك_انظمة_الملفات/محرك_انظمة_الملفات.h"
fs_driver_t* get_arabfs_driver(void);



void diskfs_read(const char* name, char* buffer);


#endif
