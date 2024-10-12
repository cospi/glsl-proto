#include "gl_vertex_array.h"

#include <assert.h>
#include <stddef.h>

void gl_vertex_array_init(GlVertexArray *_this, Logger *logger)
{
	assert(_this != NULL);
	assert(logger != NULL);

	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);

	_this->logger = logger;
	_this->vertex_array = vertex_array;
	logger->log(logger, LOG_LEVEL_INFO, "Created OpenGL vertex array (%lu).", vertex_array);
}

void gl_vertex_array_fini(const GlVertexArray *_this)
{
	assert(_this != NULL);

	GLuint vertex_array = _this->vertex_array;
	glDeleteVertexArrays(1, &vertex_array);
	Logger *logger = _this->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Destroyed OpenGL vertex array (%lu).", vertex_array);
}
