#ifndef GL_GL_PROGRAM_H_
#define GL_GL_PROGRAM_H_

#include <stdint.h>

#include "gl_shader.h"

#define GL_PROGRAM_MAX_UNIFORM_COUNT 4

typedef struct GlProgram {
	Logger *logger;
	GLuint program;
	GLint uniform_locations[GL_PROGRAM_MAX_UNIFORM_COUNT];
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
void gl_program_load_uniform_location(GlProgram *_this, size_t index, const char *uniform_name);
GLint gl_program_get_uniform_location(const GlProgram *_this, size_t index);

#endif // GL_GL_PROGRAM_H_
