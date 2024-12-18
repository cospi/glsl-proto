#ifndef FILE_SYSTEM_FILE_PATH_H_
#define FILE_SYSTEM_FILE_PATH_H_

#include "../memory/allocator.h"

char *file_path_create_relative(Allocator *allocator, const char *executable_directory, const char *path);

#endif // FILE_SYSTEM_FILE_PATH_H_
