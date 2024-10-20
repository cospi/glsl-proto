#ifndef PROTO_PROTO_H_
#define PROTO_PROTO_H_

#include "../gl/gl_mesh.h"
#include "../gl/gl_program.h"
#include "../gl/gl_sprite_batch.h"
#include "../input/keycode.h"
#include "../platform/platform.h"
#include "../render/texture_font.h"

typedef struct Proto {
	const Platform *platform;

	uint16_t flags;

	GlProgram cube_program;
	GlTexture cube_texture;
	GlMesh cube_mesh;
	Vector3 cube_position;
	Vector3 cube_rotation_axis;

	GlProgram background_program;
	GlTexture background_texture;
	Sprite background_sprite;

	GlProgram font_program;
	GlTexture font_texture;
	TextureFont font;

	GlSpriteBatch sprite_batch;

	char text[1024];

	float time_sec;
} Proto;

void proto_init(Proto *_this, const Platform *platform);
void proto_fini(const Proto *_this);
void proto_tick(Proto *_this, float delta_time_sec);
void proto_key_press(Proto *_this, Keycode keycode);

#endif // PROTO_PROTO_H_
