#include "proto.h"

#include <assert.h>

#include "../math/matrix4.h"

void proto_init(Proto *_this, const Platform *platform)
{
	assert(_this != NULL);
	assert(platform != NULL);

	Logger *logger = platform->logger;
	Allocator *allocator = platform->allocator;
	FileSystem *file_system = platform->file_system;

	_this->platform = platform;

	_this->program_initialized = gl_program_init_from_files(
		&_this->program,
		logger,
		allocator,
		file_system,
		"res/shaders/test.vert",
		"res/shaders/test.frag"
	);
	if (_this->program_initialized) {
		_this->projection_uniform_location = gl_program_get_uniform_location(&_this->program, "u_projection");
	}

	Vertex2 vertices[] = {
		(Vertex2) { (Vector2) { 160.0f, 120.0f }, (Color) { 1.0f, 0.0f, 0.0f, 1.0f } },
		(Vertex2) { (Vector2) { 480.0f, 120.0f }, (Color) { 0.0f, 1.0f, 0.0f, 1.0f } },
		(Vertex2) { (Vector2) { 320.0f, 360.0f }, (Color) { 0.0f, 0.0f, 1.0f, 1.0f } }
	};
	uint16_t indices[] = { 0, 1, 2 };
	_this->mesh_initialized = gl_mesh_init(&_this->mesh, logger, vertices, 3, indices, 3);
}

void proto_fini(const Proto *_this)
{
	assert(_this != NULL);

	if (_this->mesh_initialized) {
		gl_mesh_fini(&_this->mesh);
	}

	if (_this->program_initialized) {
		gl_program_fini(&_this->program);
	}
}

void proto_tick(const Proto *_this, float delta_time_sec)
{
	assert(_this != NULL);

	Logger *logger = _this->platform->logger;
	logger->log(logger, LOG_LEVEL_INFO, "%f", (double)delta_time_sec);

	const Platform *platform = _this->platform;
	unsigned int width = platform->window_width;
	unsigned int height = platform->window_height;
	glViewport(0, 0, (GLint)width, (GLint)height);

	glClear(GL_COLOR_BUFFER_BIT);

	if (_this->program_initialized && _this->mesh_initialized) {
		gl_program_use(&_this->program);

		int32_t projection_uniform_location = _this->projection_uniform_location;
		if (projection_uniform_location != -1) {
			Matrix4 projection;
			matrix4_ortho(projection, 0.0f, (float)width, 0.0f, (float)height, 0.0f, -1000.0f);
			glUniformMatrix4fv(projection_uniform_location, 1, GL_TRUE, projection);
		}

		gl_mesh_render(&_this->mesh);
	}
}
