#ifndef RENDER_IMAGE_H_
#define RENDER_IMAGE_H_

#include <stdbool.h>
#include <stdint.h>

#include "../file_system/file_system.h"
#include "../log/logger.h"
#include "../memory/allocator.h"

typedef struct Image {
	Allocator *allocator;
	uint16_t width;
	uint16_t height;
	unsigned char *pixels;
} Image;

bool image_init_from_tga_file(
	Image *_this,
	Logger *logger,
	Allocator *allocator,
	FileSystem *file_system,
	const char *executable_directory,
	const char *relative_path
);
void image_fini(const Image *_this);

#endif // RENDER_IMAGE_H_
