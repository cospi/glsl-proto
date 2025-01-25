#include "gl_program.h"

#include <assert.h>

static char *get_program_info_log(GLuint program, Logger *logger, Allocator *allocator)
{
	assert(program != 0);
	assert(logger != NULL);
	assert(allocator != NULL);

	GLint length = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
	if (length <= 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting OpenGL program info log length failed.");
		return NULL;
	}

	char *info_log = allocator->allocate(allocator, (size_t)length);
	if (info_log == NULL) {
		return NULL;
	}

	glGetProgramInfoLog(program, length, NULL, info_log);
	info_log[length - 1] = '\0';
	return info_log;
}

static bool link_program(GLuint program, Logger *logger, Allocator *allocator)
{
	assert(program != 0);
	assert(logger != NULL);
	assert(allocator != NULL);

	glLinkProgram(program);
	GLint status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		logger->log(logger, LOG_LEVEL_ERROR, "Linking OpenGL program failed.");
		char *info_log = get_program_info_log(program, logger, allocator);
		if (info_log != NULL) {
			logger->log(logger, LOG_LEVEL_ERROR, info_log);
			allocator->free(allocator, info_log);
		}
		return false;
	}

	return true;
}

bool gl_program_init_from_shaders(
	GlProgram *_this,
	Logger *logger,
	Allocator *allocator,
	const GlShader *shaders,
	size_t shader_count
)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(allocator != NULL);
	assert(shaders != NULL);
	assert(shader_count > 0);

	GLuint program = glCreateProgram();
	if (program == 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Creating OpenGL program failed.");
		return false;
	}

	for (size_t i = 0; i < shader_count; ++i) {
		glAttachShader(program, shaders[i].shader);
	}

	bool success = link_program(program, logger, allocator);

	for (size_t i = 0; i < shader_count; ++i) {
		glDetachShader(program, shaders[i].shader);
	}

	if (!success) {
		glDeleteProgram(program);
		return false;
	}

	GLint *uniform_locations = _this->uniform_locations;
	for (size_t i = 0; i < GL_PROGRAM_MAX_UNIFORM_COUNT; ++i) {
		uniform_locations[i] = -1;
	}

	_this->logger = logger;
	_this->program = program;
	logger->log(logger, LOG_LEVEL_INFO, "Created OpenGL program (%lu).", program);
	return true;
}

bool gl_program_init_from_files(
	GlProgram *_this,
	Logger *logger,
	Allocator *allocator,
	FileSystem *file_system,
	const char *vertex_shader_path,
	const char *fragment_shader_path
)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(allocator != NULL);
	assert(file_system != NULL);
	assert(vertex_shader_path != NULL);
	assert(fragment_shader_path != NULL);

	GlShader shaders[2];

	if (!gl_shader_init_from_file(
		&shaders[0],
		logger,
		allocator,
		file_system,
		GL_VERTEX_SHADER,
		vertex_shader_path
	)) {
		return false;
	}

	if (!gl_shader_init_from_file(
		&shaders[1],
		logger,
		allocator,
		file_system,
		GL_FRAGMENT_SHADER,
		fragment_shader_path
	)) {
		gl_shader_fini(&shaders[0]);
		return false;
	}

	bool success = gl_program_init_from_shaders(_this, logger, allocator, shaders, 2);
	gl_shader_fini(&shaders[1]);
	gl_shader_fini(&shaders[0]);
	return success;
}

void gl_program_fini(const GlProgram *_this)
{
	assert(_this != NULL);

	GLuint program = _this->program;
	glDeleteProgram(program);
	Logger *logger = _this->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Destroyed OpenGL program (%lu).", program);
}

void gl_program_use(const GlProgram *_this)
{
	assert(_this != NULL);

	glUseProgram(_this->program);
}

void gl_program_load_uniform_location(GlProgram *_this, size_t index, const char *uniform_name)
{
	assert(_this != NULL);
	assert(index < GL_PROGRAM_MAX_UNIFORM_COUNT);
	assert(uniform_name != NULL);

	GLint uniform_location = glGetUniformLocation(_this->program, uniform_name);
	if (uniform_location == -1) {
		Logger *logger = _this->logger;
		logger->log(logger, LOG_LEVEL_ERROR, "Getting uniform location for \"%s\" failed.", uniform_name);
	}

	_this->uniform_locations[index] = uniform_location;
}

GLint gl_program_get_uniform_location(const GlProgram *_this, size_t index)
{
	assert(_this != NULL);
	assert(index < GL_PROGRAM_MAX_UNIFORM_COUNT);

	return _this->uniform_locations[index];
}
