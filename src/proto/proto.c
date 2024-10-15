#include "proto.h"

#include "../math/math.h"
#include "../math/matrix4.h"
#include "../render/cube.h"

#define FOV_RADIANS (60.0f * DEGREES_TO_RADIANS)
#define NEAR_PLANE 0.0f
#define FAR_PLANE -1000.0f

static bool is_valid_uniform_location(int32_t uniform_location)
{
	return uniform_location != -1;
}

static void set_uniform_matrix4(int32_t uniform_location, const Matrix4 matrix)
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

static void proto_init_cube_program(Proto *_this)
{
	assert(_this != NULL);

	GlProgram *program = &_this->cube_program;
	if ((_this->cube_program_initialized = proto_init_program(
		_this,
		program,
		"res/shaders/cube.vert",
		"res/shaders/cube.frag"
	))) {
		_this->cube_transform_uniform_location = gl_program_get_uniform_location(program, "u_transform");
	}
}

static void proto_init_cube_texture(Proto *_this)
{
	assert(_this != NULL);

	_this->cube_texture_initialized = proto_init_texture(_this, &_this->cube_texture, "res/textures/cube.tga");
}

static void proto_init_cube_mesh(Proto *_this)
{
	assert(_this != NULL);

	Vertex3 vertices[CUBE_VERTEX_COUNT];
	uint16_t indices[CUBE_INDEX_COUNT];
	cube_init(vertices, indices);
	_this->cube_mesh_initialized = gl_mesh_init(
		&_this->cube_mesh,
		_this->platform->logger,
		vertices,
		CUBE_VERTEX_COUNT,
		indices,
		CUBE_INDEX_COUNT
	);
}

static void proto_init_sprite_batch_program(Proto *_this)
{
	assert(_this != NULL);

	GlProgram *program = &_this->sprite_batch_program;
	if ((_this->sprite_batch_program_initialized = proto_init_program(
		_this,
		program,
		"res/shaders/sprite_batch.vert",
		"res/shaders/sprite_batch.frag"
	))) {
		_this->sprite_batch_projection_uniform_location =
			gl_program_get_uniform_location(program, "u_projection");
	}
}

static void proto_init_background_texture(Proto *_this)
{
	assert(_this != NULL);

	_this->background_texture_initialized =
		proto_init_texture(_this, &_this->background_texture, "res/textures/background.tga");
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

static void proto_setup_viewport(Proto *_this)
{
	assert(_this != NULL);

	const Platform *platform = _this->platform;
	glViewport(0, 0, (GLint)platform->window_width, (GLint)platform->window_height);
}

static void proto_setup_cube_transform(Proto *_this)
{
	assert(_this != NULL);

	int32_t transform_uniform_location = _this->cube_transform_uniform_location;
	if (!is_valid_uniform_location(transform_uniform_location)) {
		return;
	}

	const Platform *platform = _this->platform;

	Matrix4 translate, rotate, model, projection, transform;
	matrix4_translate(translate, _this->cube_position);
	matrix4_rotate(rotate, _this->cube_rotation_axis, _this->time_sec);
	matrix4_product(model, translate, rotate);
	matrix4_perspective(
		projection,
		FOV_RADIANS,
		(float)platform->window_width / (float)platform->window_height,
		NEAR_PLANE,
		FAR_PLANE
	);
	matrix4_product(transform, projection, model);
	set_uniform_matrix4(transform_uniform_location, transform);
}

static void proto_render_cube(Proto *_this)
{
	assert(_this != NULL);

	if (!_this->cube_program_initialized || !_this->cube_texture_initialized || !_this->cube_mesh_initialized) {
		return;
	}

	gl_program_use(&_this->cube_program);
	proto_setup_cube_transform(_this);
	gl_texture_bind(&_this->cube_texture);
	gl_mesh_render(&_this->cube_mesh);
}

static void proto_setup_sprite_batch_projection(Proto *_this, const Matrix4 projection)
{
	assert(_this != NULL);
	assert(projection != NULL);

	int32_t projection_uniform_location = _this->sprite_batch_projection_uniform_location;
	if (is_valid_uniform_location(projection_uniform_location)) {
		set_uniform_matrix4(projection_uniform_location, projection);
	}
}

static void proto_setup_background_projection(Proto *_this)
{
	assert(_this != NULL);

	const Rect2 *rect = &_this->background_sprite.rect;
	Vector2 rect_min, rect_max;
	rect2_min_max(rect, &rect_min, &rect_max);

	Matrix4 projection;
	matrix4_orthographic(projection, rect_min.x, rect_max.x, rect_min.y, rect_max.y, NEAR_PLANE, FAR_PLANE);
	proto_setup_sprite_batch_projection(_this, projection);
}

static void proto_render_background(Proto *_this)
{
	assert(_this != NULL);

	if (
		!_this->sprite_batch_program_initialized
		|| !_this->sprite_batch_initialized
		|| !_this->background_texture_initialized
	) {
		return;
	}

	GlSpriteBatch *sprite_batch = &_this->sprite_batch;
	if (!gl_sprite_batch_start_push_sprites(sprite_batch)) {
		return;
	}

	gl_sprite_batch_push_sprite(sprite_batch, &_this->background_sprite);
	gl_sprite_batch_end_push_sprites(sprite_batch);

	gl_program_use(&_this->sprite_batch_program);
	proto_setup_background_projection(_this);
	gl_texture_bind(&_this->background_texture);
	gl_sprite_batch_render(sprite_batch);
}

static void proto_setup_font_projection(Proto *_this)
{
	assert(_this != NULL);

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
	proto_setup_sprite_batch_projection(_this, projection);
}

static void proto_render_text(Proto *_this)
{
	assert(_this != NULL);

	if (
		!_this->sprite_batch_program_initialized
		|| !_this->sprite_batch_initialized
		|| !_this->font_texture_initialized
	) {
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

	gl_program_use(&_this->sprite_batch_program);
	proto_setup_font_projection(_this);
	gl_texture_bind(&_this->font_texture);
	gl_sprite_batch_render(sprite_batch);
}

void proto_init(Proto *_this, const Platform *platform)
{
	assert(_this != NULL);
	assert(platform != NULL);

	_this->platform = platform;

	proto_init_cube_program(_this);
	proto_init_cube_texture(_this);
	proto_init_cube_mesh(_this);
	_this->cube_position = (Vector3) { 0.0f, 0.0f, -2.0f };
	_this->cube_rotation_axis = vector3_normalized((Vector3) { 1.0f, 1.0f, 0.0f });

	proto_init_sprite_batch_program(_this);
	proto_init_sprite_batch(_this);

	proto_init_background_texture(_this);
	_this->background_sprite = (Sprite) {
		(Rect2) { (Vector2) { 0.0f, 0.0f }, (Vector2) { 1.0f, 1.0f } },
		(Rect2) { (Vector2) { 0.0f, 0.0f }, (Vector2) { 1.0f, 1.0f } }
	};

	proto_init_font(_this);

	_this->time_sec = 0.0f;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void proto_fini(const Proto *_this)
{
	assert(_this != NULL);

	if (_this->font_texture_initialized) {
		gl_texture_fini(&_this->font_texture);
	}

	if (_this->background_texture_initialized) {
		gl_texture_fini(&_this->background_texture);
	}

	if (_this->sprite_batch_initialized) {
		gl_sprite_batch_fini(&_this->sprite_batch);
	}

	if (_this->sprite_batch_program_initialized) {
		gl_program_fini(&_this->sprite_batch_program);
	}

	if (_this->cube_mesh_initialized) {
		gl_mesh_fini(&_this->cube_mesh);
	}

	if (_this->cube_texture_initialized) {
		gl_texture_fini(&_this->cube_texture);
	}

	if (_this->cube_program_initialized) {
		gl_program_fini(&_this->cube_program);
	}
}

void proto_tick(Proto *_this, float delta_time_sec)
{
	assert(_this != NULL);

	_this->time_sec += delta_time_sec;

	proto_setup_viewport(_this);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	proto_render_background(_this);
	glClear(GL_DEPTH_BUFFER_BIT);
	proto_render_cube(_this);
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

	if (_this->background_texture_initialized) {
		gl_texture_fini(&_this->background_texture);
	}

	if (_this->sprite_batch_program_initialized) {
		_this->sprite_batch_projection_uniform_location = -1;
		gl_program_fini(&_this->sprite_batch_program);
	}

	if (_this->cube_texture_initialized) {
		gl_texture_fini(&_this->cube_texture);
	}

	if (_this->cube_program_initialized) {
		_this->cube_transform_uniform_location = -1;
		gl_program_fini(&_this->cube_program);
	}

	proto_init_cube_program(_this);
	proto_init_cube_texture(_this);
	proto_init_sprite_batch_program(_this);
	proto_init_background_texture(_this);
	proto_init_font(_this);

	_this->time_sec = 0.0f;
}
