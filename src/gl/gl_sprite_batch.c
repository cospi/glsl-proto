#include "gl_sprite_batch.h"

#include <assert.h>

static uint16_t *create_indices(Allocator *allocator, size_t sprite_capacity, size_t *out_indices_size)
{
	assert(allocator != NULL);
	assert(sprite_capacity > 0);
	assert(out_indices_size != NULL);

	size_t indices_size = sizeof(uint16_t) * sprite_capacity * 6;
	uint16_t *indices = allocator->allocate(allocator, indices_size);
	if (indices == NULL) {
		return NULL;
	}

	for (size_t i = 0; i < sprite_capacity; ++i) {
		uint16_t vertex_index = (uint16_t)(i * 4);
		size_t indices_offset = i * 6;
		indices[indices_offset + 5] = indices[indices_offset] = vertex_index;
		indices[indices_offset + 1] = (uint16_t)(vertex_index + 1);
		indices[indices_offset + 3] = indices[indices_offset + 2] = (uint16_t)(vertex_index + 2);
		indices[indices_offset + 4] = (uint16_t)(vertex_index + 3);
	}

	*out_indices_size = indices_size;
	return indices;
}

bool gl_sprite_batch_init(GlSpriteBatch *_this, Logger *logger, Allocator *allocator, size_t sprite_capacity)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(allocator != NULL);
	assert(sprite_capacity > 0);

	uint16_t *indices;
	size_t indices_size;
	GlVertexArray *vertex_array;
	GlBuffer *vertex_buffer;
	GlBuffer *index_buffer;

	indices = create_indices(allocator, sprite_capacity, &indices_size);
	if (indices == NULL) {
		return false;
	}

	vertex_array = &_this->vertex_array;
	gl_vertex_array_init(vertex_array, logger);
	glBindVertexArray(vertex_array->vertex_array);

	vertex_buffer = &_this->vertex_buffer;
	gl_buffer_init(vertex_buffer, logger);
	gl_buffer_bind(vertex_buffer, GL_ARRAY_BUFFER);
	gl_clear_errors();
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sprite_capacity * sizeof(Vertex2) * 4), NULL, GL_DYNAMIC_DRAW);
	if (glGetError() != GL_NO_ERROR) {
		goto error_vertex_buffer_fini;
	}

	index_buffer = &_this->index_buffer;
	gl_buffer_init(index_buffer, logger);
	gl_buffer_bind(index_buffer, GL_ELEMENT_ARRAY_BUFFER);
	gl_clear_errors();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)indices_size, indices, GL_STATIC_DRAW);
	if (glGetError() != GL_NO_ERROR) {
		goto error_index_buffer_fini;
	}

	allocator->free(allocator, indices);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (void *)offsetof(Vertex2, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (void *)offsetof(Vertex2, uv));

	_this->logger = logger;
	_this->sprite_capacity = sprite_capacity;
	_this->sprite_count = 0;
	_this->mapped_vertex_buffer = NULL;
	return true;

error_index_buffer_fini:
	gl_buffer_fini(index_buffer);
error_vertex_buffer_fini:
	gl_buffer_fini(vertex_buffer);
	gl_vertex_array_fini(vertex_array);
	allocator->free(allocator, indices);
	return false;
}

void gl_sprite_batch_fini(const GlSpriteBatch *_this)
{
	assert(_this != NULL);

	gl_buffer_fini(&_this->index_buffer);
	gl_buffer_fini(&_this->vertex_buffer);
	gl_vertex_array_fini(&_this->vertex_array);
}

bool gl_sprite_batch_start_push_sprites(GlSpriteBatch *_this)
{
	assert(_this != NULL);
	assert(_this->mapped_vertex_buffer == NULL);

	gl_buffer_bind(&_this->vertex_buffer, GL_ARRAY_BUFFER);

	void *buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (buffer == NULL) {
		Logger *logger = _this->logger;
		logger->log(logger, LOG_LEVEL_ERROR, "Mapping vertex buffer failed.");
		return false;
	}

	_this->sprite_count = 0;
	_this->mapped_vertex_buffer = buffer;
	return true;
}

void gl_sprite_batch_push_sprite(GlSpriteBatch *_this, const Sprite *sprite)
{
	assert(_this != NULL);
	assert(_this->sprite_count < _this->sprite_capacity);
	assert(_this->mapped_vertex_buffer != NULL);
	assert(sprite != NULL);

	const Rect2 *rect = &sprite->rect;
	Vector2 rect_min, rect_max;
	rect2_min_max(rect, &rect_min, &rect_max);

	const Rect2 *uv = &sprite->uv;
	Vector2 uv_min, uv_max;
	rect2_min_max(uv, &uv_min, &uv_max);

	Vertex2 *vertices = _this->mapped_vertex_buffer + (_this->sprite_count * 4);
	vertices[0] = (Vertex2) { rect_min, uv_min };
	vertices[1] = (Vertex2) { (Vector2) { rect_max.x, rect_min.y }, (Vector2) { uv_max.x, uv_min.y } };
	vertices[2] = (Vertex2) { rect_max, uv_max };
	vertices[3] = (Vertex2) { (Vector2) { rect_min.x, rect_max.y }, (Vector2) { uv_min.x, uv_max.y } };

	++_this->sprite_count;
}

void gl_sprite_batch_push_text(
	GlSpriteBatch *_this,
	const TextureFont *font,
	const char *text,
	Vector2 position,
	float scale,
	float newline_offset
)
{
	assert(_this != NULL);
	assert(font != NULL);
	assert(text != NULL);

	Vector2 character_size = vector2_scale(&font->character_size, scale);
	Sprite character_sprite;
	character_sprite.rect = (Rect2) {
		(Vector2) { position.x, position.y - character_size.y },
		character_size
	};
	character_sprite.uv.size = font->character_uv_size;

	char character;
	while ((character = *(text++)) != '\0') {
		if (character == '\n') {
			character_sprite.rect.position.x = position.x;
			character_sprite.rect.position.y -= character_size.y + newline_offset;
			continue;
		}

		character_sprite.uv.position = texture_font_get_character_uv(font, character);
		gl_sprite_batch_push_sprite(_this, &character_sprite);
		character_sprite.rect.position.x += character_size.x;
	}
}

void gl_sprite_batch_end_push_sprites(GlSpriteBatch *_this)
{
	assert(_this != NULL);
	assert(_this->mapped_vertex_buffer != NULL);

	glUnmapBuffer(GL_ARRAY_BUFFER);
	_this->mapped_vertex_buffer = NULL;
}

void gl_sprite_batch_render(const GlSpriteBatch *_this)
{
	assert(_this != NULL);

	glBindVertexArray(_this->vertex_array.vertex_array);
	glDrawElements(GL_TRIANGLES, (GLsizei)(_this->sprite_count * 6), GL_UNSIGNED_SHORT, NULL);
}
