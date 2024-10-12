#ifndef PLATFORM_FILE_SYSTEM_STDLIB_STDLIB_FILE_SYSTEM_H_
#define PLATFORM_FILE_SYSTEM_STDLIB_STDLIB_FILE_SYSTEM_H_

#include "../../../file_system/file_system.h"
#include "../../../log/logger.h"

typedef struct StdlibFileSystem {
	FileSystem base;
	Logger *logger;
} StdlibFileSystem;

void stdlib_file_system_init(StdlibFileSystem *_this, Logger *logger);

#endif // PLATFORM_FILE_SYSTEM_STDLIB_STDLIB_FILE_SYSTEM_H_
