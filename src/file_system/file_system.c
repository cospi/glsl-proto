#include "file_system.h"

#include <assert.h>

#include "file_path.h"

FileHandle file_system_open_file_relative(FileSystem *_this, Allocator *allocator, const char *path)
{
	assert(_this != NULL);
	assert(path != NULL);
	assert(allocator != NULL);

	char *file_path = file_path_create_relative(allocator, _this->get_executable_directory(_this), path);
	if (file_path == NULL) {
		return NULL;
	}

	FileHandle file = _this->open_file(_this, file_path);
	allocator->free(allocator, file_path);
	return file;
}
