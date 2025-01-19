#include "texture_font.h"

#include <limits.h>

void texture_font_init(
	TextureFont *_this,
	const GlTexture *texture,
	size_t characters_per_row,
	size_t characters_per_column
)
{
	assert(_this != NULL);
	assert(texture != NULL);
	assert(characters_per_row > 0);
	assert(characters_per_column > 0);

	float characters_per_row_inverse_f = 1.0f / (float)characters_per_row;
	float characters_per_column_inverse_f = 1.0f / (float)characters_per_column;

	_this->texture = texture;
	_this->characters_per_row = characters_per_row;
	_this->character_size = (Vector2) {
		(float)texture->width * characters_per_row_inverse_f,
		(float)texture->height * characters_per_column_inverse_f
	};
	_this->character_uv_size = (Vector2) {
		characters_per_row_inverse_f,
		characters_per_column_inverse_f
	};
}

Vector2 texture_font_get_character_uv(const TextureFont *_this, char character)
{
	assert(_this != NULL);

	size_t character_unsigned = (size_t)character;
	size_t characters_per_row = _this->characters_per_row;
	const Vector2 *character_uv_size = &_this->character_uv_size;
	return (Vector2) {
		(float)(character_unsigned % characters_per_row) * character_uv_size->x,
		(float)(UCHAR_MAX - (character_unsigned / characters_per_row)) * character_uv_size->y
	};
}
