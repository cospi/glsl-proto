#include "stdlib_file_system.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

static const char *stdlib_file_system_get_executable_directory(const FileSystem *_this)
{
	assert(_this != NULL);
	assert(((uintptr_t)(const void *)_this % _Alignof(StdlibFileSystem)) == 0);

	return ((const StdlibFileSystem *)_this)->executable_directory;
}

static FileHandle stdlib_file_system_open_file(FileSystem *_this, const char *path)
{
	assert(_this != NULL);
	assert(((uintptr_t)(void *)_this % _Alignof(StdlibFileSystem)) == 0);
	assert(path != NULL);

	Logger *logger = ((const StdlibFileSystem *)_this)->logger;

	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "Opening file \"%s\" failed.", path);
		return NULL;
	}

	FileHandle handle = (FileHandle)file;
	logger->log(logger, LOG_LEVEL_INFO, "Opened file \"%s\" (%" PRIxPTR ").", path, (uintptr_t)handle);
	return handle;
}

static void stdlib_file_system_close_file(FileSystem *_this, FileHandle handle)
{
	assert(_this != NULL);
	assert(((uintptr_t)(void *)_this % _Alignof(StdlibFileSystem)) == 0);
	assert(handle != NULL);

	fclose((FILE *)handle);
	Logger *logger = ((const StdlibFileSystem *)_this)->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Closed file (%" PRIxPTR ").", (uintptr_t)handle);
}

static bool stdlib_file_system_try_get_file_size(FileSystem *_this, FileHandle handle, size_t *out_size)
{
	assert(_this != NULL);
	assert(((uintptr_t)(void *)_this % _Alignof(StdlibFileSystem)) == 0);
	assert(handle != NULL);

	Logger *logger = ((const StdlibFileSystem *)_this)->logger;

	FILE *file = (FILE *)handle;
	long original_position = ftell(file);
	if (original_position < 0L) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting file position failed.");
		return false;
	}

	if (fseek(file, 0L, SEEK_END) != 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Navigating to end of file failed.");
		return false;
	}

	long size = ftell(file);
	if (fseek(file, original_position, SEEK_SET) != 0) {
		logger->log(logger, LOG_LEVEL_WARNING, "Navigating to original file position failed.");
	}
	if (size < 0L) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting file size failed.");
		return false;
	}

	*out_size = (size_t)size;
	return true;
}

static bool stdlib_file_system_try_read_file(FileSystem *_this, FileHandle handle, unsigned char *buffer, size_t size)
{
	assert(_this != NULL);
	assert(((uintptr_t)(void *)_this % _Alignof(StdlibFileSystem)) == 0);
	assert(handle != NULL);
	assert(buffer != NULL);
	assert(size > 0);

	if (fread(buffer, size, 1, (FILE *)handle) != 1) {
		Logger *logger = ((const StdlibFileSystem *)_this)->logger;
		logger->log(logger, LOG_LEVEL_ERROR, "Reading %zu bytes from file failed.", size);
		return false;
	}

	return true;
}

void stdlib_file_system_init(StdlibFileSystem *_this, Logger *logger, const char *executable_directory)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(executable_directory != NULL);

	_this->base.get_executable_directory = stdlib_file_system_get_executable_directory;
	_this->base.open_file = stdlib_file_system_open_file;
	_this->base.close_file = stdlib_file_system_close_file;
	_this->base.try_get_file_size = stdlib_file_system_try_get_file_size;
	_this->base.try_read_file = stdlib_file_system_try_read_file;
	_this->logger = logger;
	_this->executable_directory = executable_directory;
}
