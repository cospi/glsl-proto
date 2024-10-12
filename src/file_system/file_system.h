#ifndef FILE_SYSTEM_FILE_SYSTEM_H_
#define FILE_SYSTEM_FILE_SYSTEM_H_

#include <stdbool.h>
#include <stddef.h>

typedef void *FileHandle;

typedef struct FileSystem FileSystem;

struct FileSystem {
	FileHandle (*open_file)(FileSystem *_this, const char *path);
	void (*close_file)(FileSystem *_this, FileHandle handle);
	bool (*try_get_file_size)(FileSystem *_this, FileHandle handle, size_t *out_size);
	bool (*try_read_file)(FileSystem *_this, FileHandle handle, unsigned char *buffer, size_t size);
};

#endif // FILE_SYSTEM_FILE_SYSTEM_H_
