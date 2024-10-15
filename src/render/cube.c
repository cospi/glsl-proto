#include "cube.h"

#include <assert.h>
#include <stddef.h>

static void init_cube_face_vertices(
	Vertex3 *vertices,
	size_t vertices_offset,
	Vector3 bottom_left,
	Vector3 right,
	Vector3 up,
	Vector3 normal
)
{
	assert(vertices != NULL);

	Vector3 direction = vector3_sum(&right, &up);
	vertices[vertices_offset] = (Vertex3) {
		bottom_left,
		normal,
		(Vector2) { 0.0f, 0.0f }
	};
	vertices[vertices_offset + 1] = (Vertex3) {
		vector3_sum(&bottom_left, &right),
		normal,
		(Vector2) { 1.0f, 0.0f }
	};
	vertices[vertices_offset + 2] = (Vertex3) {
		vector3_sum(&bottom_left, &direction),
		normal,
		(Vector2) { 1.0f, 1.0f }
	};
	vertices[vertices_offset + 3] = (Vertex3) {
		vector3_sum(&bottom_left, &up),
		normal,
		(Vector2) { 0.0f, 1.0f }
	};
}

static void init_cube_vertices(Vertex3 *vertices)
{
	assert(vertices != NULL);

	// \note Left
	init_cube_face_vertices(
		vertices,
		0,
		(Vector3) { -0.5f, -0.5f, -0.5f },
		(Vector3) { 0.0f, 0.0f, 1.0f },
		(Vector3) { 0.0f, 1.0f, 0.0f },
		(Vector3) { -1.0f, 0.0f, 0.0f }
	);

	// \note Right
	init_cube_face_vertices(
		vertices,
		4,
		(Vector3) { 0.5f, -0.5f, 0.5f },
		(Vector3) { 0.0f, 0.0f, -1.0f },
		(Vector3) { 0.0f, 1.0f, 0.0f },
		(Vector3) { 1.0f, 0.0f, 0.0f }
	);

	// \note Top
	init_cube_face_vertices(
		vertices,
		8,
		(Vector3) { -0.5f, 0.5f, 0.5f },
		(Vector3) { 1.0f, 0.0f, 0.0f },
		(Vector3) { 0.0f, 0.0f, -1.0f },
		(Vector3) { 0.0f, 1.0f, 0.0f }
	);

	// \note Bottom
	init_cube_face_vertices(
		vertices,
		12,
		(Vector3) { -0.5f, -0.5f, -0.5f },
		(Vector3) { 1.0f, 0.0f, 0.0f },
		(Vector3) { 0.0f, 0.0f, 1.0f },
		(Vector3) { 0.0f, -1.0f, 0.0f }
	);

	// \note Front
	init_cube_face_vertices(
		vertices,
		16,
		(Vector3) { -0.5f, -0.5f, 0.5f },
		(Vector3) { 1.0f, 0.0f, 0.0f },
		(Vector3) { 0.0f, 1.0f, 0.0f },
		(Vector3) { 0.0f, 0.0f, 1.0f }
	);

	// \note Back
	init_cube_face_vertices(
		vertices,
		20,
		(Vector3) { 0.5f, -0.5f, -0.5f },
		(Vector3) { -1.0f, 0.0f, 0.0f },
		(Vector3) { 0.0f, 1.0f, 0.0f },
		(Vector3) { 0.0f, 0.0f, -1.0f }
	);
}

static void init_cube_face_indices(uint16_t *indices, size_t indices_offset, size_t vertex_index)
{
	assert(indices != NULL);

	indices[indices_offset + 5] = indices[indices_offset] = (uint16_t)vertex_index;
	indices[indices_offset + 1] = (uint16_t)(vertex_index + 1);
	indices[indices_offset + 3] = indices[indices_offset + 2] = (uint16_t)(vertex_index + 2);
	indices[indices_offset + 4] = (uint16_t)(vertex_index + 3);
}

static void init_cube_indices(uint16_t *indices)
{
	assert(indices != NULL);

	for (size_t i = 0; i < 6; ++i) {
		init_cube_face_indices(indices, i * 6, i * 4);
	}
}

void cube_init(Vertex3 *vertices, uint16_t *indices)
{
	assert(vertices != NULL);
	assert(indices != NULL);

	init_cube_vertices(vertices);
	init_cube_indices(indices);
}
