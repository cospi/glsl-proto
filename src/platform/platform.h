#ifndef PLATFORM_PLATFORM_H_
#define PLATFORM_PLATFORM_H_

#include "../file_system/file_system.h"
#include "../log/logger.h"
#include "../memory/allocator.h"

typedef struct Platform {
	Logger *logger;
	Allocator *allocator;
	FileSystem *file_system;
} Platform;

#endif // PLATFORM_PLATFORM_H_
