#ifndef GL_GL_MESH_H_
#define GL_GL_MESH_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gl_buffer.h"
#include "gl_vertex_array.h"
#include "../render/vertex2.h"

typedef struct GlMesh {
	GlVertexArray vertex_array;
	GlBuffer vertex_buffer;
	GlBuffer index_buffer;
	GLsizei index_count;
} GlMesh;

bool gl_mesh_init(
	GlMesh *_this,
	Logger *logger,
	const Vertex2 *vertices,
	size_t vertex_count,
	const uint16_t *indices,
	size_t index_count
);
void gl_mesh_fini(const GlMesh *_this);
void gl_mesh_render(const GlMesh *_this);

#endif // GL_GL_MESH_H_
