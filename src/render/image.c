#include "image.h"

#include <assert.h>
#include <string.h>

#include "../file_system/file.h"

static const unsigned char UNCOMPRESSED_TGA_HEADER[] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static uint16_t parse_uint16_little_endian(const unsigned char *data)
{
	assert(data != NULL);

	return (uint16_t)(data[0] | (data[1] << 8));
}

bool image_init_from_tga_file(
	Image *_this,
	Logger *logger,
	Allocator *allocator,
	FileSystem *file_system,
	const char *path
)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(allocator != NULL);
	assert(file_system != NULL);
	assert(path != NULL);

	File file;
	size_t size;
	unsigned char header[18];
	unsigned char bytes_per_pixel;
	uint16_t width;
	uint16_t height;
	unsigned char *pixels;
	unsigned char *pixel;

	if (!file_init_relative(&file, allocator, file_system, path)) {
		return false;
	}

	if (!file_try_get_size(&file, &size)) {
		goto error_fini_file;
	}

	if (!file_try_read(&file, header, sizeof header)) {
		goto error_fini_file;
	}

	if (memcmp(header, UNCOMPRESSED_TGA_HEADER, sizeof UNCOMPRESSED_TGA_HEADER) != 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Unsupported TGA header.");
		goto error_fini_file;
	}

	bytes_per_pixel = header[16] >> 3;
	if ((bytes_per_pixel != 3 /* BGR */) && (bytes_per_pixel != 4 /* BGRA */)) {
		logger->log(logger, LOG_LEVEL_ERROR, "Unsupported TGA bytes per pixel.");
		goto error_fini_file;
	}

	width = parse_uint16_little_endian(header + 12);
	height = parse_uint16_little_endian(header + 14);

	pixels = allocator->allocate(allocator, (uint32_t)width * (uint32_t)height * 4);
	if (pixels == NULL) {
		goto error_fini_file;
	}

	pixel = pixels;
	for (uint16_t y = 0; y < height; ++y) {
		for (uint16_t x = 0; x < width; ++x) {
			unsigned char tmp;

			pixel[3] = 0xFF;
			if (!file_try_read(&file, pixel, bytes_per_pixel)) {
				allocator->free(allocator, pixels);
				goto error_fini_file;
			}

			// \note Convert BGR(A) to RGB(A).
			tmp = pixel[0];
			pixel[0] = pixel[2];
			pixel[2] = tmp;
			pixel += 4;
		}
	}

	file_fini(&file);

	_this->allocator = allocator;
	_this->width = width;
	_this->height = height;
	_this->pixels = pixels;
	return true;

error_fini_file:
	file_fini(&file);
	return false;
}

void image_fini(const Image *_this)
{
	assert(_this != NULL);

	Allocator *allocator = _this->allocator;
	allocator->free(allocator, _this->pixels);
}
