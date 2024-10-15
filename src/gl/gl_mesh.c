#include "gl_mesh.h"

#include <assert.h>

bool gl_mesh_init(
	GlMesh *_this,
	Logger *logger,
	const Vertex2 *vertices,
	size_t vertex_count,
	const uint16_t *indices,
	size_t index_count
)
{
	assert(_this != NULL);
	assert(vertices != NULL);
	assert(vertex_count > 0);
	assert(indices != NULL);
	assert(index_count > 0);

	GlVertexArray *vertex_array;
	GlBuffer *vertex_buffer;
	GlBuffer *index_buffer;

	vertex_array = &_this->vertex_array;
	gl_vertex_array_init(vertex_array, logger);
	glBindVertexArray(vertex_array->vertex_array);

	vertex_buffer = &_this->vertex_buffer;
	gl_buffer_init(vertex_buffer, logger);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->buffer);
	gl_clear_errors();
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertex_count * sizeof(Vertex2)), vertices, GL_STATIC_DRAW);
	if (glGetError() != GL_NO_ERROR) {
		goto error_vertex_buffer_fini;
	}

	index_buffer = &_this->index_buffer;
	gl_buffer_init(index_buffer, logger);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->buffer);
	gl_clear_errors();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(index_count * sizeof(uint16_t)), indices, GL_STATIC_DRAW);
	if (glGetError() != GL_NO_ERROR) {
		goto error_index_buffer_fini;
	}

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (void *)offsetof(Vertex2, position));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (void *)offsetof(Vertex2, uv));

	_this->index_count = (GLsizei)index_count;
	return true;

error_index_buffer_fini:
	gl_buffer_fini(index_buffer);
error_vertex_buffer_fini:
	gl_buffer_fini(vertex_buffer);
	gl_vertex_array_fini(vertex_array);
	return false;
}

void gl_mesh_fini(const GlMesh *_this)
{
	assert(_this != NULL);

	gl_buffer_fini(&_this->index_buffer);
	gl_buffer_fini(&_this->vertex_buffer);
	gl_vertex_array_fini(&_this->vertex_array);
}

void gl_mesh_render(const GlMesh *_this)
{
	assert(_this != NULL);

	glBindVertexArray(_this->vertex_array.vertex_array);
	glDrawElements(GL_TRIANGLES, _this->index_count, GL_UNSIGNED_SHORT, NULL);
}
