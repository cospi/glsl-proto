#ifndef PROTO_PROTO_H_
#define PROTO_PROTO_H_

#include "../gl/gl_mesh.h"
#include "../gl/gl_program.h"
#include "../gl/gl_sprite_batch.h"
#include "../platform/platform.h"
#include "../render/texture_font.h"

typedef struct Proto {
	const Platform *platform;

	GlProgram cube_program;
	bool cube_program_initialized;
	int32_t cube_transform_uniform_location;

	GlTexture cube_texture;
	bool cube_texture_initialized;

	GlMesh cube_mesh;
	bool cube_mesh_initialized;

	Vector3 cube_position;
	Vector3 cube_rotation_axis;

	GlProgram sprite_batch_program;
	bool sprite_batch_program_initialized;
	int32_t sprite_batch_projection_uniform_location;

	GlSpriteBatch sprite_batch;
	bool sprite_batch_initialized;

	GlTexture background_texture;
	bool background_texture_initialized;

	Sprite background_sprite;

	GlTexture font_texture;
	bool font_texture_initialized;
	TextureFont font;

	float time_sec;
} Proto;

void proto_init(Proto *_this, const Platform *platform);
void proto_fini(const Proto *_this);
void proto_tick(Proto *_this, float delta_time_sec);
void proto_reload(Proto *_this);

#endif // PROTO_PROTO_H_
