#include "file_path.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

char *file_path_create(Allocator *allocator, const char *executable_directory, const char *relative_path)
{
	assert(allocator != NULL);
	assert(executable_directory != NULL);
	assert(relative_path != NULL);

	size_t executable_directory_length = strlen(executable_directory);
	size_t full_path_length = executable_directory_length + strlen(relative_path);
	char *full_path = allocator->allocate(allocator, full_path_length + 1);
	if (full_path == NULL) {
		return NULL;
	}

	sprintf(full_path, "%s", executable_directory);
	sprintf(full_path + executable_directory_length, "%s", relative_path);
	full_path[full_path_length] = '\0';
	return full_path;
}
