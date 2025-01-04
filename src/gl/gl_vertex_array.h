#ifndef GL_GL_VERTEX_ARRAY_H_
#define GL_GL_VERTEX_ARRAY_H_

#include "gl.h"
#include "../log/logger.h"

typedef struct GlVertexArray {
	Logger *logger;
	GLuint vertex_array;
} GlVertexArray;

void gl_vertex_array_init(GlVertexArray *_this, Logger *logger);
void gl_vertex_array_fini(const GlVertexArray *_this);
void gl_vertex_array_bind(const GlVertexArray *_this);

#endif // GL_GL_VERTEX_ARRAY_H_
