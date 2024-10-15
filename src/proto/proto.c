#include "proto.h"

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

	_this->texture_initialized = gl_texture_init_from_tga_file(
		&_this->texture,
		logger,
		allocator,
		file_system,
		"res/textures/test.tga"
	);

	_this->sprite_batch_initialized = gl_sprite_batch_init(&_this->sprite_batch, logger, allocator, 4);

	Vertex2 vertices[] = {
		(Vertex2) { (Vector2) { 160.0f, 120.0f }, (Vector2) { 0.0f, 0.0f } },
		(Vertex2) { (Vector2) { 480.0f, 120.0f }, (Vector2) { 1.0f, 0.0f } },
		(Vertex2) { (Vector2) { 480.0f, 360.0f }, (Vector2) { 1.0f, 1.0f } },
		(Vertex2) { (Vector2) { 160.0f, 360.0f }, (Vector2) { 0.0f, 1.0f } }
	};
	uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };
	_this->mesh_initialized = gl_mesh_init(&_this->mesh, logger, vertices, 4, indices, 6);
}

void proto_fini(const Proto *_this)
{
	assert(_this != NULL);

	if (_this->mesh_initialized) {
		gl_mesh_fini(&_this->mesh);
	}

	if (_this->sprite_batch_initialized) {
		gl_sprite_batch_fini(&_this->sprite_batch);
	}

	if (_this->texture_initialized) {
		gl_texture_fini(&_this->texture);
	}

	if (_this->program_initialized) {
		gl_program_fini(&_this->program);
	}
}

void proto_tick(Proto *_this, float delta_time_sec)
{
	assert(_this != NULL);

	Logger *logger = _this->platform->logger;
	logger->log(logger, LOG_LEVEL_INFO, "%f", (double)delta_time_sec);

	const Platform *platform = _this->platform;
	unsigned int width = platform->window_width;
	unsigned int height = platform->window_height;
	glViewport(0, 0, (GLint)width, (GLint)height);

	glClear(GL_COLOR_BUFFER_BIT);

	if (!_this->program_initialized) {
		return;
	}

	gl_program_use(&_this->program);

	int32_t projection_uniform_location = _this->projection_uniform_location;
	if (projection_uniform_location != -1) {
		Matrix4 projection;
		matrix4_ortho(projection, 0.0f, (float)width, 0.0f, (float)height, 0.0f, -1000.0f);
		glUniformMatrix4fv(projection_uniform_location, 1, GL_TRUE, projection);
	}

	if (_this->texture_initialized) {
		gl_texture_bind(&_this->texture);
	}

	if (_this->mesh_initialized) {
		gl_mesh_render(&_this->mesh);
	}

	if (_this->sprite_batch_initialized) {
		GlSpriteBatch *sprite_batch = &_this->sprite_batch;
		if (gl_sprite_batch_start_push_sprites(sprite_batch)) {
			for (int i = 0;i < 4; ++i) {
				float f = (float)i;
				float offset_x = f * 80.0f;
				float offset_y = f * 60.0f;
				Sprite sprite = (Sprite) {
					(Rect2) {
						(Vector2) { 160.0f + offset_x, 120.0f + offset_y },
						(Vector2) { 80.0f, 60.0f }
					},
					(Rect2) {
						(Vector2) { 0.0f, 0.0f },
						(Vector2) { 1.0f, 1.0f }
					}
				};
				gl_sprite_batch_push_sprite(sprite_batch, &sprite);
			}
			gl_sprite_batch_end_push_sprites(sprite_batch);
			gl_sprite_batch_render(sprite_batch);
		}
	}
}
