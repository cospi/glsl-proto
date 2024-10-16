#ifndef PLATFORM_FILE_SYSTEM_EXECUTABLE_LINUX_LINUX_EXECUTABLE_H_
#define PLATFORM_FILE_SYSTEM_EXECUTABLE_LINUX_LINUX_EXECUTABLE_H_

#include "../../../../log/logger.h"
#include "../../../../memory/allocator.h"

char *linux_get_executable_directory(Logger *logger, Allocator *allocator);

#endif // PLATFORM_FILE_SYSTEM_EXECUTABLE_LINUX_LINUX_EXECUTABLE_H_
