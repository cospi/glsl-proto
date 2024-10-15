#include "gl_buffer.h"

#include <assert.h>
#include <stddef.h>

void gl_buffer_init(GlBuffer *_this, Logger *logger)
{
	assert(_this != NULL);
	assert(logger != NULL);

	GLuint buffer;
	glGenBuffers(1, &buffer);

	_this->logger = logger;
	_this->buffer = buffer;
	logger->log(logger, LOG_LEVEL_INFO, "Created OpenGL buffer (%lu).", buffer);
}

void gl_buffer_fini(const GlBuffer *_this)
{
	assert(_this != NULL);

	GLuint buffer = _this->buffer;
	glDeleteBuffers(1, &buffer);
	Logger *logger = _this->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Destroyed OpenGL buffer (%lu).", buffer);
}

void gl_buffer_bind(const GlBuffer *_this, GLenum target)
{
	assert(_this != NULL);

	glBindBuffer(target, _this->buffer);
}
