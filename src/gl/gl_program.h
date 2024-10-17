#ifndef GL_GL_PROGRAM_H_
#define GL_GL_PROGRAM_H_

#include <stdint.h>

#include "gl_shader.h"

typedef struct GlProgram {
	Logger *logger;
	GLuint program;
} GlProgram;

bool gl_program_init_from_shaders(
	GlProgram *_this,
	Logger *logger,
	Allocator *allocator,
	const GlShader *shaders,
	size_t shader_count
);
bool gl_program_init_from_files(
	GlProgram *_this,
	Logger *logger,
	Allocator *allocator,
	FileSystem *file_system,
	const char *vertex_shader_path,
	const char *fragment_shader_path
);
void gl_program_fini(const GlProgram *_this);
void gl_program_use(const GlProgram *_this);
int32_t gl_program_get_uniform_location(const GlProgram *_this, const char *uniform_name);

#endif // GL_GL_PROGRAM_H_
