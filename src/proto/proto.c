#include "proto.h"

#include <stdio.h>

#include "../math/math.h"
#include "../math/matrix4.h"
#include "../render/cube.h"

#define CUBE_PROGRAM_INIT_FLAG 0x01
#define CUBE_TEXTURE_INIT_FLAG 0x02
#define CUBE_MESH_INIT_FLAG 0x04
#define CUBE_RENDER_FLAGS (CUBE_PROGRAM_INIT_FLAG | CUBE_TEXTURE_INIT_FLAG | CUBE_MESH_INIT_FLAG)

#define BACKGROUND_PROGRAM_INIT_FLAG 0x08
#define BACKGROUND_TEXTURE_INIT_FLAG 0x10
#define BACKGROUND_RENDER_FLAGS (BACKGROUND_PROGRAM_INIT_FLAG | BACKGROUND_TEXTURE_INIT_FLAG)

#define FONT_PROGRAM_INIT_FLAG 0x20
#define FONT_TEXTURE_INIT_FLAG 0x40
#define TEXT_RENDER_FLAGS (FONT_PROGRAM_INIT_FLAG | FONT_TEXTURE_INIT_FLAG)

#define SPRITE_BATCH_INIT_FLAG 0x80

#define RELOAD_FLAGS ( \
	CUBE_PROGRAM_INIT_FLAG \
	| CUBE_TEXTURE_INIT_FLAG \
	| BACKGROUND_PROGRAM_INIT_FLAG \
	| BACKGROUND_TEXTURE_INIT_FLAG \
)

#define FOV_RADIANS (60.0f * DEGREES_TO_RADIANS)
#define NEAR_PLANE 0.0f
#define FAR_PLANE -1000.0f
#define TEXT_POSITION 8.0f
#define TEXT_SCALE 2.0f
#define NEWLINE_OFFSET 8.0f

static bool is_valid_uniform_location(GLint uniform_location)
{
	return uniform_location != -1;
}

static void set_uniform_matrix4(GLint uniform_location, const Matrix4 matrix)
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
	if (proto_init_program(_this, program, "res/shaders/cube.vert", "res/shaders/cube.frag")) {
		gl_program_load_uniform_location(program, 0, "u_transform");
		_this->flags |= CUBE_PROGRAM_INIT_FLAG;
	}
}

static void proto_init_cube_texture(Proto *_this)
{
	assert(_this != NULL);

	if (proto_init_texture(_this, &_this->cube_texture, "res/textures/cube.tga")) {
		_this->flags |= CUBE_TEXTURE_INIT_FLAG;
	}
}

static void proto_init_cube_mesh(Proto *_this)
{
	assert(_this != NULL);

	Vertex3 vertices[CUBE_VERTEX_COUNT];
	uint16_t indices[CUBE_INDEX_COUNT];
	cube_init(vertices, indices);
	if (gl_mesh_init(
		&_this->cube_mesh,
		_this->platform->logger,
		vertices,
		CUBE_VERTEX_COUNT,
		indices,
		CUBE_INDEX_COUNT
	)) {
		_this->flags |= CUBE_MESH_INIT_FLAG;
	}
}

static void proto_init_background_program(Proto *_this)
{
	assert(_this != NULL);

	GlProgram *program = &_this->background_program;
	if (proto_init_program(_this, program, "res/shaders/background.vert", "res/shaders/background.frag")) {
		gl_program_load_uniform_location(program, 0, "u_projection");
		_this->flags |= BACKGROUND_PROGRAM_INIT_FLAG;
	}
}

static void proto_init_background_texture(Proto *_this)
{
	assert(_this != NULL);

	if (proto_init_texture(_this, &_this->background_texture, "res/textures/background.tga")) {
		_this->flags |= BACKGROUND_TEXTURE_INIT_FLAG;
	}
}

static void proto_init_font_program(Proto *_this)
{
	assert(_this != NULL);

	GlProgram *program = &_this->font_program;
	if (proto_init_program(
		_this,
		program,
		"res/shaders/font.vert",
		"res/shaders/font.frag"
	)) {
		gl_program_load_uniform_location(program, 0, "u_projection");
		_this->flags |= FONT_PROGRAM_INIT_FLAG;
	}
}

static void proto_init_font(Proto *_this)
{
	assert(_this != NULL);

	GlTexture *texture = &_this->font_texture;
	if (proto_init_texture(_this, texture, "res/textures/font.tga")) {
		texture_font_init(&_this->font, texture, 16, 16);
		_this->flags |= FONT_TEXTURE_INIT_FLAG;
	}
}

static void proto_init_sprite_batch(Proto *_this)
{
	assert(_this != NULL);

	const Platform *platform = _this->platform;
	if (gl_sprite_batch_init(&_this->sprite_batch, platform->logger, platform->allocator, 1024)) {
		_this->flags |= SPRITE_BATCH_INIT_FLAG;
	}
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

	GLint transform_uniform_location = gl_program_get_uniform_location(&_this->cube_program, 0);
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

	if ((_this->flags & CUBE_RENDER_FLAGS) != CUBE_RENDER_FLAGS) {
		return;
	}

	gl_program_use(&_this->cube_program);
	proto_setup_cube_transform(_this);
	gl_texture_bind(&_this->cube_texture);
	gl_mesh_render(&_this->cube_mesh);
}

static void proto_setup_background_projection(Proto *_this)
{
	assert(_this != NULL);

	GLint projection_uniform_location = gl_program_get_uniform_location(&_this->background_program, 0);
	if (!is_valid_uniform_location(projection_uniform_location)) {
		return;
	}

	const Rect2 *rect = &_this->background_sprite.rect;
	Vector2 rect_min, rect_max;
	rect2_min_max(rect, &rect_min, &rect_max);

	Matrix4 projection;
	matrix4_orthographic(projection, rect_min.x, rect_max.x, rect_min.y, rect_max.y, NEAR_PLANE, FAR_PLANE);
	set_uniform_matrix4(projection_uniform_location, projection);
}

static void proto_render_background(Proto *_this)
{
	assert(_this != NULL);

	if ((_this->flags & BACKGROUND_RENDER_FLAGS) != BACKGROUND_RENDER_FLAGS) {
		return;
	}

	GlSpriteBatch *sprite_batch = &_this->sprite_batch;
	if (!gl_sprite_batch_start_push_sprites(sprite_batch)) {
		return;
	}

	gl_sprite_batch_push_sprite(sprite_batch, &_this->background_sprite);
	gl_sprite_batch_end_push_sprites(sprite_batch);

	gl_program_use(&_this->background_program);
	proto_setup_background_projection(_this);
	gl_texture_bind(&_this->background_texture);
	gl_sprite_batch_render(sprite_batch);
}

static void proto_setup_font_projection(Proto *_this)
{
	assert(_this != NULL);

	GLint projection_uniform_location = gl_program_get_uniform_location(&_this->font_program, 0);
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
		NEAR_PLANE,
		FAR_PLANE
	);
	set_uniform_matrix4(projection_uniform_location, projection);
}

static void proto_render_text(Proto *_this, float delta_time_sec)
{
	assert(_this != NULL);

	if ((_this->flags & TEXT_RENDER_FLAGS) != TEXT_RENDER_FLAGS) {
		return;
	}

	GlSpriteBatch *sprite_batch = &_this->sprite_batch;
	if (!gl_sprite_batch_start_push_sprites(sprite_batch)) {
		return;
	}

	sprintf(_this->text, "%d FPS", (int)(1.0f / delta_time_sec));

	gl_sprite_batch_push_text(
		sprite_batch,
		&_this->font,
		_this->text,
		(Vector2) { TEXT_POSITION, (float)_this->platform->window_height - TEXT_POSITION },
		TEXT_SCALE,
		NEWLINE_OFFSET
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
	_this->flags = 0;

	proto_init_cube_program(_this);
	proto_init_cube_texture(_this);
	proto_init_cube_mesh(_this);
	_this->cube_position = (Vector3) { 0.0f, 0.0f, -2.0f };
	_this->cube_rotation_axis = vector3_normalized((Vector3) { 1.0f, 1.0f, 0.0f });

	proto_init_background_program(_this);
	proto_init_background_texture(_this);
	_this->background_sprite = (Sprite) {
		(Rect2) { (Vector2) { 0.0f, 0.0f }, (Vector2) { 1.0f, 1.0f } },
		(Rect2) { (Vector2) { 0.0f, 0.0f }, (Vector2) { 1.0f, 1.0f } }
	};

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

	unsigned char flags = _this->flags;

	if ((flags & SPRITE_BATCH_INIT_FLAG) == SPRITE_BATCH_INIT_FLAG) {
		gl_sprite_batch_fini(&_this->sprite_batch);
	}

	if ((flags & FONT_TEXTURE_INIT_FLAG) == FONT_TEXTURE_INIT_FLAG) {
		gl_texture_fini(&_this->font_texture);
	}

	if ((flags & FONT_PROGRAM_INIT_FLAG) == FONT_PROGRAM_INIT_FLAG) {
		gl_program_fini(&_this->font_program);
	}

	if ((flags & BACKGROUND_TEXTURE_INIT_FLAG) == BACKGROUND_TEXTURE_INIT_FLAG) {
		gl_texture_fini(&_this->background_texture);
	}

	if ((flags & BACKGROUND_PROGRAM_INIT_FLAG) == BACKGROUND_PROGRAM_INIT_FLAG) {
		gl_program_fini(&_this->background_program);
	}

	if ((flags & CUBE_MESH_INIT_FLAG) == CUBE_MESH_INIT_FLAG) {
		gl_mesh_fini(&_this->cube_mesh);
	}

	if ((flags & CUBE_TEXTURE_INIT_FLAG) == CUBE_TEXTURE_INIT_FLAG) {
		gl_texture_fini(&_this->cube_texture);
	}

	if ((flags & CUBE_PROGRAM_INIT_FLAG) == CUBE_PROGRAM_INIT_FLAG) {
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
	proto_render_text(_this, delta_time_sec);
}

void proto_reload(Proto *_this)
{
	assert(_this != NULL);

	Logger *logger = _this->platform->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Reloading textures and shaders...");

	unsigned char flags = _this->flags;

	if ((flags & BACKGROUND_TEXTURE_INIT_FLAG) == BACKGROUND_TEXTURE_INIT_FLAG) {
		gl_texture_fini(&_this->background_texture);
	}

	if ((flags & BACKGROUND_PROGRAM_INIT_FLAG) == BACKGROUND_PROGRAM_INIT_FLAG) {
		gl_program_fini(&_this->background_program);
	}

	if ((flags & CUBE_TEXTURE_INIT_FLAG) == CUBE_TEXTURE_INIT_FLAG) {
		gl_texture_fini(&_this->cube_texture);
	}

	if ((flags & CUBE_PROGRAM_INIT_FLAG) == CUBE_PROGRAM_INIT_FLAG) {
		gl_program_fini(&_this->cube_program);
	}

	_this->flags = (unsigned char)(flags & ~RELOAD_FLAGS);

	proto_init_cube_program(_this);
	proto_init_cube_texture(_this);
	proto_init_background_program(_this);
	proto_init_background_texture(_this);

	_this->time_sec = 0.0f;
}
