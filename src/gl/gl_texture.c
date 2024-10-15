#include "gl_texture.h"

#include <assert.h>

bool gl_texture_init_from_image(GlTexture *_this, Logger *logger, const Image *image)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(image != NULL);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gl_clear_errors();
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		(GLsizei)image->width,
		(GLsizei)image->height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image->pixels
	);
	if (glGetError() != GL_NO_ERROR) {
		logger->log(logger, LOG_LEVEL_ERROR, "Creating OpenGL texture failed.");
		glDeleteTextures(1, &texture);
		return false;
	}

	_this->logger = logger;
	_this->texture = texture;
	logger->log(logger, LOG_LEVEL_INFO, "Created OpenGL texture (%lu).", texture);
	return true;
}

bool gl_texture_init_from_tga_file(
	GlTexture *_this,
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

	Image image;
	if (!image_init_from_tga_file(&image, logger, allocator, file_system, path)) {
		return false;
	}

	bool success = gl_texture_init_from_image(_this, logger, &image);
	image_fini(&image);
	return success;
}

void gl_texture_fini(const GlTexture *_this)
{
	assert(_this != NULL);

	GLuint texture = _this->texture;
	glDeleteTextures(1, &texture);
	Logger *logger = _this->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Destroyed OpenGL texture (%lu).", texture);
}

void gl_texture_bind(const GlTexture *_this)
{
	assert(_this != NULL);

	glBindTexture(GL_TEXTURE_2D, _this->texture);
}
