#include "gl_shader.h"

#include <assert.h>

static char *get_shader_info_log(GLuint shader, Logger *logger, Allocator *allocator)
{
	assert(shader != 0);
	assert(logger != NULL);
	assert(allocator != NULL);

	GLint length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	if (length <= 0) {
		logger->log(logger, LOG_LEVEL_INFO, "Getting OpenGL shader info log length failed.");
		return NULL;
	}

	char *info_log = allocator->allocate(allocator, (size_t)length);
	if (info_log == NULL) {
		return NULL;
	}

	glGetShaderInfoLog(shader, length, NULL, info_log);
	info_log[length - 1] = '\0';
	return info_log;
}

static bool compile_shader(GLuint shader, Logger *logger, Allocator *allocator)
{
	assert(shader != 0);
	assert(logger != NULL);
	assert(allocator != NULL);

	glCompileShader(shader);
	GLint status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		logger->log(logger, LOG_LEVEL_ERROR, "Compiling OpenGL shader failed.");
		char *info_log = get_shader_info_log(shader, logger, allocator);
		if (info_log != NULL) {
			logger->log(logger, LOG_LEVEL_ERROR, info_log);
			allocator->free(allocator, info_log);
		}
		return false;
	}

	return true;
}

static char *read_shader_source(Allocator *allocator, FileSystem *file_system, const char *path)
{
	assert(allocator != NULL);
	assert(file_system != NULL);
	assert(path != NULL);

	FileHandle file;
	size_t size;
	char *source;

	file = file_system->open_file(file_system, path);
	if (file == NULL) {
		return NULL;
	}

	if (!file_system->try_get_file_size(file_system, file, &size)) {
		goto error_close_file;
	}

	source = allocator->allocate(allocator, size + 1);
	if (source == NULL) {
		goto error_close_file;
	}

	if (!file_system->try_read_file(file_system, file, (unsigned char *)source, size)) {
		allocator->free(allocator, source);
		goto error_close_file;
	}

	source[size] = '\0';
	file_system->close_file(file_system, file);
	return source;

error_close_file:
	file_system->close_file(file_system, file);
	return NULL;
}

bool gl_shader_init_from_source(
	GlShader *_this,
	Logger *logger,
	Allocator *allocator,
	GLenum type,
	const char *source
)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(allocator != NULL);
	assert(source != NULL);

	GLuint shader = glCreateShader(type);
	if (shader == 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Creating OpenGL shader failed.");
		return false;
	}

	glShaderSource(shader, 1, &source, NULL);
	if (!compile_shader(shader, logger, allocator)) {
		glDeleteShader(shader);
		return false;
	}

	_this->logger = logger;
	_this->shader = shader;
	logger->log(logger, LOG_LEVEL_INFO, "Created OpenGL shader (%lu).", shader);
	return true;
}

bool gl_shader_init_from_file(
	GlShader *_this,
	Logger *logger,
	Allocator *allocator,
	FileSystem *file_system,
	GLenum type,
	const char *path
)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(allocator != NULL);
	assert(file_system != NULL);
	assert(path != NULL);

	char *source = read_shader_source(allocator, file_system, path);
	if (source == NULL) {
		return false;
	}

	bool success = gl_shader_init_from_source(_this, logger, allocator, type, source);
	allocator->free(allocator, source);
	return success;
}

void gl_shader_fini(const GlShader *_this)
{
	assert(_this != NULL);

	GLuint shader = _this->shader;
	glDeleteShader(shader);
	Logger *logger = _this->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Destroyed OpenGL shader (%lu).", shader);
}
