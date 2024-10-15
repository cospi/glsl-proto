#ifndef GL_GL_BUFFER_H_
#define GL_GL_BUFFER_H_

#include "../log/logger.h"
#include "gl.h"

typedef struct GlBuffer {
	Logger *logger;
	GLuint buffer;
} GlBuffer;

void gl_buffer_init(GlBuffer *_this, Logger *logger);
void gl_buffer_fini(const GlBuffer *_this);
void gl_buffer_bind(const GlBuffer *_this, GLenum target);

#endif // GL_GL_BUFFER_H_
