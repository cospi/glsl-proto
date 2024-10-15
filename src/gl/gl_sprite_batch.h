#ifndef GL_GL_SPRITE_BATCH_H_
#define GL_GL_SPRITE_BATCH_H_

#include <stdbool.h>

#include "gl_buffer.h"
#include "gl_vertex_array.h"
#include "../memory/allocator.h"
#include "../render/sprite.h"
#include "../render/vertex2.h"

typedef struct GlSpriteBatch {
	Logger *logger;
	GlVertexArray vertex_array;
	GlBuffer vertex_buffer;
	GlBuffer index_buffer;
	size_t sprite_capacity;
	size_t sprite_count;
	Vertex2 *mapped_vertex_buffer;
} GlSpriteBatch;

bool gl_sprite_batch_init(GlSpriteBatch *_this, Logger *logger, Allocator *allocator, size_t sprite_capacity);
void gl_sprite_batch_fini(const GlSpriteBatch *_this);
bool gl_sprite_batch_start_push_sprites(GlSpriteBatch *_this);
void gl_sprite_batch_push_sprite(GlSpriteBatch *_this, const Sprite *sprite);
void gl_sprite_batch_end_push_sprites(GlSpriteBatch *_this);
void gl_sprite_batch_render(const GlSpriteBatch *_this);

#endif // GL_GL_SPRITE_BATCH_H_
