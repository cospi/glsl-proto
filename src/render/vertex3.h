#ifndef RENDER_VERTEX3_H_
#define RENDER_VERTEX3_H_

#include "../math/vector3.h"
#include "../math/vector2.h"

typedef struct Vertex3 {
	Vector3 position;
	Vector3 normal;
	Vector2 uv;
} Vertex3;

#endif // RENDER_VERTEX3_H_
