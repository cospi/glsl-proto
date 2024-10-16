#ifndef GL_GL_SHADER_H_
#define GL_GL_SHADER_H_

#include <stdbool.h>

#include "../file_system/file_system.h"
#include "../log/logger.h"
#include "../memory/allocator.h"
#include "gl.h"

typedef struct GlShader {
	Logger *logger;
	GLuint shader;
} GlShader;

bool gl_shader_init_from_source(
	GlShader *_this,
	Logger *logger,
	Allocator *allocator,
	GLenum type,
	const char *source
);
bool gl_shader_init_from_file(
	GlShader *_this,
	Logger *logger,
	Allocator *allocator,
	FileSystem *file_system,
	GLenum type,
	const char *executable_directory,
	const char *relative_path
);
void gl_shader_fini(const GlShader *_this);

#endif // GL_GL_SHADER_H_
