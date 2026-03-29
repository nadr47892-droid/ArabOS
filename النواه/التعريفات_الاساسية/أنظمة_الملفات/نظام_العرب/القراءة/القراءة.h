#ifndef ARABFS_READ_H
#define ARABFS_READ_H

void diskfs_read(const char* path, char* buffer);
const char* diskfs_read_wrapper(const char* path, char* buffer);

#endif
