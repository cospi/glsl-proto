#include "linux_executable.h"

#include <unistd.h>
#include <linux/limits.h>

#include <assert.h>

static char *get_executable_path(Logger *logger, Allocator *allocator, ssize_t *out_length)
{
	assert(logger != NULL);
	assert(allocator != NULL);
	assert(out_length != NULL);

	char *executable_path = allocator->allocate(allocator, PATH_MAX);
	if (executable_path == NULL) {
		return NULL;
	}

	ssize_t length = readlink("/proc/self/exe", executable_path, PATH_MAX - 1);
	if (length <= 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting executable path failed.");
		allocator->free(allocator, executable_path);
		return NULL;
	}

	executable_path[length] = '\0';
	*out_length = length;
	return executable_path;
}

static ssize_t get_separator_index(const char *executable_path, ssize_t length)
{
	assert(executable_path != NULL);
	assert(length > 0);

	for (ssize_t i = length - 1; i >= 0; --i) {
		if (executable_path[i] == '/') {
			return i;
		}
	}

	return -1;
}

char *linux_get_executable_directory(Logger *logger, Allocator *allocator)
{
	assert(logger != NULL);
	assert(allocator != NULL);

	ssize_t length;
	char *executable_directory = get_executable_path(logger, allocator, &length);
	if (executable_directory == NULL) {
		return NULL;
	}

	ssize_t separator_index = get_separator_index(executable_directory, length);
	if (separator_index == -1) {
		allocator->free(allocator, executable_directory);
		return NULL;
	}

	executable_directory[separator_index + 1] = '\0';
	return executable_directory;
}
