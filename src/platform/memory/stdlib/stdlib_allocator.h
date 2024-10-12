#ifndef PLATFORM_MEMORY_STDLIB_STDLIB_ALLOCATOR_H_
#define PLATFORM_MEMORY_STDLIB_STDLIB_ALLOCATOR_H_

#include "../../../memory/allocator.h"
#include "../../../log/logger.h"

typedef struct StdlibAllocator {
	Allocator base;
	Logger *logger;
} StdlibAllocator;

void stdlib_allocator_init(StdlibAllocator *_this, Logger *logger);

#endif // PLATFORM_MEMORY_STDLIB_STDLIB_ALLOCATOR_H_
