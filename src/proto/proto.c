#include "proto.h"

#include <assert.h>

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

	Vertex2 vertices[] = {
		(Vertex2) { (Vector2) { -0.5f, -0.5f }, (Color) { 1.0f, 0.0f, 0.0f, 1.0f } },
		(Vertex2) { (Vector2) { 0.5f, -0.5f }, (Color) { 0.0f, 1.0f, 0.0f, 1.0f } },
		(Vertex2) { (Vector2) { 0.0f, 0.5f }, (Color) { 0.0f, 0.0f, 1.0f, 1.0f } }
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

	glClear(GL_COLOR_BUFFER_BIT);
	if (_this->program_initialized && _this->mesh_initialized) {
		gl_program_use(&_this->program);
		gl_mesh_render(&_this->mesh);
	}
}
