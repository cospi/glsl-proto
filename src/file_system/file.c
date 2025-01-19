#include "file.h"

#include <assert.h>

#include "file_path.h"

bool file_init(File *_this, FileSystem *file_system, const char *path)
{
	assert(_this != NULL);
	assert(file_system != NULL);
	assert(path != NULL);

	FileHandle handle = file_system->open_file(file_system, path);
	if (handle == NULL) {
		return false;
	}

	_this->file_system = file_system;
	_this->handle = handle;
	return true;
}

bool file_init_relative(File *_this, Allocator *allocator, FileSystem *file_system, const char *path)
{
	assert(_this != NULL);
	assert(allocator != NULL);
	assert(file_system != NULL);
	assert(path != NULL);

	char *file_path =
		file_path_create_relative(allocator, file_system->get_executable_directory(file_system), path);
	if (file_path == NULL) {
		return false;
	}

	bool success = file_init(_this, file_system, file_path);
	allocator->free(allocator, file_path);
	return success;
}

void file_fini(const File *_this)
{
	assert(_this != NULL);

	FileSystem *file_system = _this->file_system;
	file_system->close_file(file_system, _this->handle);
}

bool file_try_get_size(const File *_this, size_t *out_size)
{
	assert(_this != NULL);
	assert(out_size != NULL);

	FileSystem *file_system = _this->file_system;
	return file_system->try_get_file_size(file_system, _this->handle, out_size);
}

bool file_try_read(const File *_this, unsigned char *buffer, size_t size)
{
	assert(_this != NULL);
	assert(buffer != NULL);
	assert(size > 0);

	FileSystem *file_system = _this->file_system;
	return file_system->try_read_file(file_system, _this->handle, buffer, size);
}
