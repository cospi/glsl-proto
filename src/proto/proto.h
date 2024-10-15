#ifndef PROTO_PROTO_H_
#define PROTO_PROTO_H_

#include "../gl/gl_mesh.h"
#include "../gl/gl_program.h"
#include "../gl/gl_sprite_batch.h"
#include "../gl/gl_texture.h"
#include "../platform/platform.h"

typedef struct Proto {
	const Platform *platform;

	GlProgram program;
	bool program_initialized;
	int32_t projection_uniform_location;

	GlTexture  texture;
	bool texture_initialized;

	GlSpriteBatch sprite_batch;
	bool sprite_batch_initialized;

	GlMesh mesh;
	bool mesh_initialized;
} Proto;

void proto_init(Proto *_this, const Platform *platform);
void proto_fini(const Proto *_this);
void proto_tick(Proto *_this, float delta_time_sec);

#endif // PROTO_PROTO_H_
