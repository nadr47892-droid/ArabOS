#ifndef ARABFS_FILES_H
#define ARABFS_FILES_H

int diskfs_create(const char* path, const char* data);
int diskfs_exists_wrapper(const char* path);
int diskfs_delete(const char* path);

#endif
