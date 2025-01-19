#ifndef FILE_SYSTEM_FILE_H_
#define FILE_SYSTEM_FILE_H_

#include "file_system.h"
#include "../memory/allocator.h"

typedef struct File {
	FileSystem *file_system;
	FileHandle handle;
} File;

bool file_init(File *_this, FileSystem *file_system, const char *path);
bool file_init_relative(File *_this, Allocator *allocator, FileSystem *file_system, const char *path);
void file_fini(const File *_this);
bool file_try_get_size(const File *_this, size_t *out_size);
bool file_try_read(const File *_this, unsigned char *buffer, size_t size);

#endif // FILE_SYSTEM_FILE_H_
