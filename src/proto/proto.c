#include "proto.h"

#include "../math/matrix4.h"

static void proto_init_program(Proto *_this)
{
	assert(_this != NULL);

	const Platform *platform = _this->platform;
	GlProgram *program = &_this->program;
	if ((_this->program_initialized = gl_program_init_from_files(
		program,
		platform->logger,
		platform->allocator,
		platform->file_system,
		"res/shaders/test.vert",
		"res/shaders/test.frag"
	))) {
		_this->projection_uniform_location = gl_program_get_uniform_location(program, "u_projection");
	}
}

static bool proto_init_texture(Proto *_this, GlTexture *texture, const char *path)
{
	assert(_this != NULL);
	assert(texture != NULL);
	assert(path != NULL);

	const Platform *platform = _this->platform;
	return gl_texture_init_from_tga_file(
		texture,
		platform->logger,
		platform->allocator,
		platform->file_system,
		path
	);
}

static void proto_init_mesh_texture(Proto *_this)
{
	assert(_this != NULL);

	_this->mesh_texture_initialized = proto_init_texture(_this, &_this->mesh_texture, "res/textures/test.tga");
}

static void proto_init_font(Proto *_this)
{
	assert(_this != NULL);

	GlTexture *texture = &_this->font_texture;
	if ((_this->font_texture_initialized = proto_init_texture(_this, texture, "res/textures/font.tga"))) {
		texture_font_init(&_this->font, texture, 16, 16);
	}
}

static void proto_init_sprite_batch(Proto *_this)
{
	assert(_this != NULL);

	const Platform *platform = _this->platform;
	_this->sprite_batch_initialized =
		gl_sprite_batch_init(&_this->sprite_batch, platform->logger, platform->allocator, 1024);
}

static void proto_init_mesh(Proto *_this)
{
	assert(_this != NULL);

	Vertex2 vertices[] = {
		(Vertex2) { (Vector2) { 160.0f, 120.0f }, (Vector2) { 0.0f, 0.0f } },
		(Vertex2) { (Vector2) { 480.0f, 120.0f }, (Vector2) { 1.0f, 0.0f } },
		(Vertex2) { (Vector2) { 480.0f, 360.0f }, (Vector2) { 1.0f, 1.0f } },
		(Vertex2) { (Vector2) { 160.0f, 360.0f }, (Vector2) { 0.0f, 1.0f } }
	};
	uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };
	_this->mesh_initialized = gl_mesh_init(&_this->mesh, _this->platform->logger, vertices, 4, indices, 6);
}

void proto_init(Proto *_this, const Platform *platform)
{
	assert(_this != NULL);
	assert(platform != NULL);

	_this->platform = platform;
	proto_init_program(_this);
	proto_init_mesh_texture(_this);
	proto_init_font(_this);
	proto_init_sprite_batch(_this);
	proto_init_mesh(_this);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

	if (_this->font_texture_initialized) {
		gl_texture_fini(&_this->font_texture);
	}

	if (_this->mesh_texture_initialized) {
		gl_texture_fini(&_this->mesh_texture);
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
		matrix4_orthographic(projection, 0.0f, (float)width, 0.0f, (float)height, 0.0f, -1000.0f);
		glUniformMatrix4fv(projection_uniform_location, 1, GL_TRUE, projection);
	}

	if (_this->mesh_texture_initialized && _this->mesh_initialized) {
		gl_texture_bind(&_this->mesh_texture);
		gl_mesh_render(&_this->mesh);
	}

	if (_this->font_texture_initialized && _this->sprite_batch_initialized) {
		gl_texture_bind(&_this->font_texture);

		GlSpriteBatch *sprite_batch = &_this->sprite_batch;
		if (gl_sprite_batch_start_push_sprites(sprite_batch)) {
			gl_sprite_batch_push_text(
				sprite_batch,
				&_this->font,
				"Testing text rendering\nAnother line\nAnd another one",
				(Vector2) { 8.0f, (float)height - 8.0f },
				2.0f,
				8.0f
			);
			gl_sprite_batch_end_push_sprites(sprite_batch);
			gl_sprite_batch_render(sprite_batch);
		}
	}
}

void proto_reload(Proto *_this)
{
	assert(_this != NULL);

	Logger *logger = _this->platform->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Reloading textures and shaders...");

	if (_this->font_texture_initialized) {
		gl_texture_fini(&_this->font_texture);
	}

	if (_this->mesh_texture_initialized) {
		gl_texture_fini(&_this->mesh_texture);
	}

	if (_this->program_initialized) {
		_this->projection_uniform_location = -1;
		gl_program_fini(&_this->program);
	}

	proto_init_program(_this);
	proto_init_mesh_texture(_this);
	proto_init_font(_this);
}
