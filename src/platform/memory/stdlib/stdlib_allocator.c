#include "stdlib_allocator.h"

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>

static void *stdlib_allocator_allocate(Allocator *_this, size_t size)
{
	assert(_this != NULL);
	assert(((uintptr_t)(void *)_this % _Alignof(StdlibAllocator)) == 0);
	assert(size > 0);

	Logger *logger = ((const StdlibAllocator *)_this)->logger;

	void *memory = malloc(size);
	if (memory == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "Allocating %zu bytes of memory failed.", size);
		return NULL;
	}

	logger->log(logger, LOG_LEVEL_INFO, "Allocated %zu bytes of memory (%" PRIxPTR ").", size, (uintptr_t)memory);
	return memory;
}

static void stdlib_allocator_free(Allocator *_this, void *memory)
{
	assert(_this != NULL);
	assert(((uintptr_t)(void *)_this % _Alignof(StdlibAllocator)) == 0);
	assert(memory != NULL);

	// \note Prevent -Wuse-after-free=3 from firing.
	uintptr_t memory_location = (uintptr_t)memory;
	free(memory);
	Logger *logger = ((const StdlibAllocator *)_this)->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Freed memory (%" PRIxPTR ").", memory_location);
}

void stdlib_allocator_init(StdlibAllocator *_this, Logger *logger)
{
	assert(_this != NULL);
	assert(logger != NULL);

	_this->base.allocate = stdlib_allocator_allocate;
	_this->base.free = stdlib_allocator_free;
	_this->logger = logger;
}
