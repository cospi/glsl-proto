#ifndef RENDER_TEXTURE_FONT_H_
#define RENDER_TEXTURE_FONT_H_

#include "../gl/gl_texture.h"
#include "../math/rect2.h"

typedef struct TextureFont {
	const GlTexture *texture;
	size_t characters_per_row;
	Vector2 character_size;
	Vector2 character_uv_size;
} TextureFont;

void texture_font_init(
	TextureFont *_this,
	const GlTexture *texture,
	size_t characters_per_row,
	size_t characters_per_column
);
Vector2 texture_font_get_character_uv(const TextureFont *_this, char character);

#endif // RENDER_TEXTURE_FONT_H_
