#ifndef PLATFORM_FILE_SYSTEM_STDLIB_STDLIB_FILE_SYSTEM_H_
#define PLATFORM_FILE_SYSTEM_STDLIB_STDLIB_FILE_SYSTEM_H_

#include "../../../file_system/file_system.h"
#include "../../../log/logger.h"

typedef struct StdlibFileSystem {
	FileSystem base;
	Logger *logger;
	const char *executable_directory;
} StdlibFileSystem;

void stdlib_file_system_init(StdlibFileSystem *_this, Logger *logger, const char *executable_directory);

#endif // PLATFORM_FILE_SYSTEM_STDLIB_STDLIB_FILE_SYSTEM_H_
