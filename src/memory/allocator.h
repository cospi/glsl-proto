#ifndef MEMORY_ALLOCATOR_H_
#define MEMORY_ALLOCATOR_H_

#include <stddef.h>

typedef struct Allocator Allocator;

struct Allocator {
	void *(*allocate)(Allocator *_this, size_t size);
	void (*free)(Allocator *_this, void *memory);
};

#endif // MEMORY_ALLOCATOR_H_
