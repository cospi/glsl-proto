#include "proto.h"

#include "../math/math.h"
#include "../math/matrix4.h"
#include "../render/cube.h"

static bool is_valid_uniform_location(int32_t uniform_location)
{
	return uniform_location != -1;
}

static void set_uniform_matrix4(int32_t uniform_location, Matrix4 matrix)
{
	assert(matrix != NULL);

	glUniformMatrix4fv(uniform_location, 1, GL_TRUE, matrix);
}

static bool proto_init_program(
	Proto *_this,
	GlProgram *program,
	const char *vertex_shader_path,
	const char *fragment_shader_path
)
{
	assert(_this != NULL);
	assert(program != NULL);
	assert(vertex_shader_path != NULL);
	assert(fragment_shader_path != NULL);

	const Platform *platform = _this->platform;
	return gl_program_init_from_files(
		program,
		platform->logger,
		platform->allocator,
		platform->file_system,
		vertex_shader_path,
		fragment_shader_path
	);
}

static void proto_init_mesh_program(Proto *_this)
{
	assert(_this != NULL);

	GlProgram *program = &_this->mesh_program;
	if ((_this->mesh_program_initialized = proto_init_program(
		_this,
		program,
		"res/shaders/mesh.vert",
		"res/shaders/mesh.frag"
	))) {
		_this->mesh_transform_uniform_location = gl_program_get_uniform_location(program, "u_transform");
	}
}

static void proto_init_font_program(Proto *_this)
{
	assert(_this != NULL);

	GlProgram *program = &_this->font_program;
	if ((_this->font_program_initialized = proto_init_program(
		_this,
		program,
		"res/shaders/font.vert",
		"res/shaders/font.frag"
	))) {
		_this->font_projection_uniform_location = gl_program_get_uniform_location(program, "u_projection");
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

	_this->mesh_texture_initialized = proto_init_texture(_this, &_this->mesh_texture, "res/textures/mesh.tga");
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

	Vertex3 vertices[CUBE_VERTEX_COUNT];
	uint16_t indices[CUBE_INDEX_COUNT];
	cube_init(vertices, indices);
	_this->mesh_initialized = gl_mesh_init(
		&_this->mesh,
		_this->platform->logger,
		vertices,
		CUBE_VERTEX_COUNT,
		indices,
		CUBE_INDEX_COUNT
	);
}

static void proto_setup_viewport(Proto *_this)
{
	assert(_this != NULL);

	const Platform *platform = _this->platform;
	glViewport(0, 0, (GLint)platform->window_width, (GLint)platform->window_height);
}

static void proto_setup_mesh_transform(Proto *_this)
{
	assert(_this != NULL);

	int32_t transform_uniform_location = _this->mesh_transform_uniform_location;
	if (!is_valid_uniform_location(transform_uniform_location)) {
		return;
	}

	const Platform *platform = _this->platform;

	Matrix4 translate, rotate, model, projection, transform;
	matrix4_translate(translate, (Vector3) { 0.0f, 0.0f, -2.0f });
	matrix4_rotate(rotate, vector3_normalized((Vector3) { 1.0f, 1.0f, 0.0f }), _this->time_sec);
	matrix4_product(model, translate, rotate);
	matrix4_perspective(
		projection,
		60.0f * DEGREES_TO_RADIANS,
		(float)platform->window_width / (float)platform->window_height,
		0.0f,
		-1000.0f
	);
	matrix4_product(transform, projection, model);
	set_uniform_matrix4(transform_uniform_location, transform);
}

static void proto_render_mesh(Proto *_this)
{
	assert(_this != NULL);

	if (!_this->mesh_program_initialized || !_this->mesh_texture_initialized || !_this->mesh_initialized) {
		return;
	}

	gl_program_use(&_this->mesh_program);
	proto_setup_mesh_transform(_this);
	gl_texture_bind(&_this->mesh_texture);
	gl_mesh_render(&_this->mesh);
}

static void proto_setup_font_projection(Proto *_this)
{
	assert(_this != NULL);

	int32_t projection_uniform_location = _this->font_projection_uniform_location;
	if (!is_valid_uniform_location(projection_uniform_location)) {
		return;
	}

	const Platform *platform = _this->platform;

	Matrix4 projection;
	matrix4_orthographic(
		projection,
		0.0f,
		(float)platform->window_width,
		0.0f,
		(float)platform->window_height,
		0.0f,
		-1000.0f
	);
	set_uniform_matrix4(projection_uniform_location, projection);
}

static void proto_render_text(Proto *_this)
{
	assert(_this != NULL);

	if (!_this->font_program_initialized || !_this->font_texture_initialized || !_this->sprite_batch_initialized) {
		return;
	}

	GlSpriteBatch *sprite_batch = &_this->sprite_batch;
	if (!gl_sprite_batch_start_push_sprites(sprite_batch)) {
		return;
	}

	gl_sprite_batch_push_text(
		sprite_batch,
		&_this->font,
		"Testing text rendering\nAnother line\nAnd another one",
		(Vector2) { 8.0f, (float)_this->platform->window_height - 8.0f },
		2.0f,
		8.0f
	);
	gl_sprite_batch_end_push_sprites(sprite_batch);
	gl_program_use(&_this->font_program);
	proto_setup_font_projection(_this);
	gl_texture_bind(&_this->font_texture);
	gl_sprite_batch_render(sprite_batch);
}

void proto_init(Proto *_this, const Platform *platform)
{
	assert(_this != NULL);
	assert(platform != NULL);

	_this->platform = platform;

	proto_init_mesh_program(_this);
	proto_init_mesh_texture(_this);
	proto_init_mesh(_this);

	proto_init_font_program(_this);
	proto_init_font(_this);
	proto_init_sprite_batch(_this);

	_this->time_sec = 0.0f;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void proto_fini(const Proto *_this)
{
	assert(_this != NULL);

	if (_this->sprite_batch_initialized) {
		gl_sprite_batch_fini(&_this->sprite_batch);
	}

	if (_this->font_texture_initialized) {
		gl_texture_fini(&_this->font_texture);
	}

	if (_this->font_program_initialized) {
		gl_program_fini(&_this->font_program);
	}

	if (_this->mesh_initialized) {
		gl_mesh_fini(&_this->mesh);
	}

	if (_this->mesh_texture_initialized) {
		gl_texture_fini(&_this->mesh_texture);
	}

	if (_this->mesh_program_initialized) {
		gl_program_fini(&_this->mesh_program);
	}
}

void proto_tick(Proto *_this, float delta_time_sec)
{
	assert(_this != NULL);

	_this->time_sec += delta_time_sec;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	proto_setup_viewport(_this);
	proto_render_mesh(_this);
	proto_render_text(_this);
}

void proto_reload(Proto *_this)
{
	assert(_this != NULL);

	Logger *logger = _this->platform->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Reloading textures and shaders...");

	if (_this->font_texture_initialized) {
		gl_texture_fini(&_this->font_texture);
	}

	if (_this->font_program_initialized) {
		_this->font_projection_uniform_location = -1;
		gl_program_fini(&_this->font_program);
	}

	if (_this->mesh_texture_initialized) {
		gl_texture_fini(&_this->mesh_texture);
	}

	if (_this->mesh_program_initialized) {
		_this->mesh_transform_uniform_location = -1;
		gl_program_fini(&_this->mesh_program);
	}

	proto_init_mesh_program(_this);
	proto_init_mesh_texture(_this);
	proto_init_font_program(_this);
	proto_init_font(_this);

	_this->time_sec = 0.0f;
}
