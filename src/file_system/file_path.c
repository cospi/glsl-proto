#include "file_path.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

char *file_path_create_relative(Allocator *allocator, const char *executable_directory, const char *path)
{
	assert(allocator != NULL);
	assert(executable_directory != NULL);
	assert(path != NULL);

	size_t executable_directory_length = strlen(executable_directory);
	size_t file_path_length = executable_directory_length + strlen(path);
	char *file_path = allocator->allocate(allocator, file_path_length + 1);
	if (file_path == NULL) {
		return NULL;
	}

	sprintf(file_path, "%s", executable_directory);
	sprintf(file_path + executable_directory_length, "%s", path);
	file_path[file_path_length] = '\0';
	return file_path;
}
