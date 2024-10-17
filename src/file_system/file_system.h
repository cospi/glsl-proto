#ifndef FILE_SYSTEM_FILE_SYSTEM_H_
#define FILE_SYSTEM_FILE_SYSTEM_H_

#include <stdbool.h>
#include <stddef.h>

#include "../memory/allocator.h"

typedef void *FileHandle;

typedef struct FileSystem FileSystem;

struct FileSystem {
	const char *(*get_executable_directory)(const FileSystem *_this);
	FileHandle (*open_file)(FileSystem *_this, const char *path);
	void (*close_file)(FileSystem *_this, FileHandle handle);
	bool (*try_get_file_size)(FileSystem *_this, FileHandle handle, size_t *out_size);
	bool (*try_read_file)(FileSystem *_this, FileHandle handle, unsigned char *buffer, size_t size);
};

FileHandle file_system_open_file_relative(FileSystem *_this, Allocator *allocator, const char *path);

#endif // FILE_SYSTEM_FILE_SYSTEM_H_
