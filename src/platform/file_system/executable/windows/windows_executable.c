#include "windows_executable.h"

#include <Windows.h>

#include <assert.h>

static char *get_executable_path(Logger *logger, Allocator *allocator, size_t *out_length)
{
	assert(logger != NULL);
	assert(allocator != NULL);
	assert(out_length != NULL);

	char *executable_path = allocator->allocate(allocator, MAX_PATH);
	if (executable_path == NULL) {
		return NULL;
	}

	DWORD length = GetModuleFileNameA(NULL, executable_path, MAX_PATH);
	if (length == 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting executable path failed.");
		allocator->free(allocator, executable_path);
		return NULL;
	}

	executable_path[length] = '\0';
	*out_length = (size_t)length;
	return executable_path;
}

static int get_separator_index(const char *executable_path, size_t length)
{
	assert(executable_path != NULL);
	assert(length > 0);

	if (length > INT_MAX) {
		return -1;
	}

	for (int i = (int)length - 1; i >= 0; --i) {
		char character = executable_path[i];

		if ((character == '/') || (character == '\\')) {
			return i;
		}
	}

	return -1;
}

char *windows_get_executable_directory(Logger *logger, Allocator *allocator)
{
	assert(logger != NULL);
	assert(allocator != NULL);

	size_t length;
	char *executable_directory = get_executable_path(logger, allocator, &length);
	if (executable_directory == NULL) {
		return NULL;
	}

	int separator_index = get_separator_index(executable_directory, length);
	if (separator_index == -1) {
		allocator->free(allocator, executable_directory);
		return NULL;
	}

	for (int i = 0; i <= separator_index; ++i) {
		if (executable_directory[i] == '\\') {
			executable_directory[i] = '/';
		}
	}

	executable_directory[separator_index + 1] = '\0';
	return executable_directory;
}
