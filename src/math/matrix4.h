#ifndef MATH_MATRIX4_H_
#define MATH_MATRIX4_H_

#include <assert.h>
#include <string.h>

typedef float Matrix4[16];

static inline void matrix4_zero(Matrix4 _this)
{
	assert(_this != NULL);

	memset(_this, 0, sizeof(Matrix4));
}

static inline void matrix4_ortho(
	Matrix4 _this,
	float left,
	float right,
	float bottom,
	float top,
	float near_plane,
	float far_plane
)
{
	assert(_this != NULL);

	matrix4_zero(_this);
	_this[0] = 2.0f / (right - left);
	_this[3] = (left + right) / (left - right);
	_this[5] = 2.0f / (top - bottom);
	_this[7] = (bottom + top) / (bottom - top);
	_this[10] = 2.0f / (near_plane - far_plane);
	_this[11] = (near_plane + far_plane) / (near_plane - far_plane);
	_this[15] = 1.0f;
}

#endif // MATH_MATRIX4_H_
