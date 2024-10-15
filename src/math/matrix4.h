#ifndef MATH_MATRIX4_H_
#define MATH_MATRIX4_H_

#include <math.h>
#include <string.h>

#include "vector3.h"

typedef float Matrix4[16];

static inline void matrix4_zero(Matrix4 _this)
{
	assert(_this != NULL);

	memset(_this, 0, sizeof(Matrix4));
}

static inline void matrix4_identity(Matrix4 _this)
{
	assert(_this != NULL);

	matrix4_zero(_this);
	_this[0] = 1.0f;
	_this[5] = 1.0f;
	_this[10] = 1.0f;
	_this[15] = 1.0f;
}

static inline void matrix4_product(Matrix4 _this, const Matrix4 left, const Matrix4 right)
{
	assert(_this != NULL);
	assert(left != NULL);
	assert(right != NULL);

	for (size_t y = 0; y < 4; ++y) {
		size_t row_offset = y * 4;
		const float *left_row = left + row_offset;
		float *result_row = _this + row_offset;
		for (size_t x = 0; x < 4; ++x) {
			float value = 0.0f;
			for (size_t i = 0; i < 4; ++i) {
				value += left_row[i] * right[(i * 4) + x];
			}
			result_row[x] = value;
		}
	}
}

static inline void matrix4_translate(Matrix4 _this, Vector3 translation)
{
	assert(_this != NULL);

	matrix4_identity(_this);
	_this[3] = translation.x;
	_this[7] = translation.y;
	_this[11] = translation.z;
}

static inline void matrix4_rotate(Matrix4 _this, Vector3 axis, float angle_radians)
{
	assert(_this != NULL);

	float c = cosf(angle_radians);
	float d = 1.0f - c;
	float s = sinf(angle_radians);
	float xx = axis.x * axis.x;
	float xy = axis.x * axis.y;
	float xz = axis.x * axis.z;
	float yy = axis.y * axis.y;
	float yz = axis.y * axis.z;
	float zz = axis.z * axis.z;
	float xs = axis.x * s;
	float ys = axis.y * s;
	float zs = axis.z * s;

	matrix4_zero(_this);
	_this[0] = (xx * d) + c;
	_this[1] = (xy * d) - zs;
	_this[2] = (xz * d) + ys;
	_this[4] = (xy * d) + zs;
	_this[5] = (yy * d) + c;
	_this[6] = (yz * d) - xs;
	_this[8] = (xz * d) - ys;
	_this[9] = (yz * d) + xs;
	_this[10] = (zz * d) + c;
	_this[15] = 1.0f;
}

static inline void matrix4_perspective(
	Matrix4 _this,
	float fov_radians,
	float aspect_ratio,
	float near_plane,
	float far_plane
)
{
	assert(_this != NULL);

	float tan_fov_half_inverse = 1.0f / (tanf(fov_radians * 0.5f));
	float near_minus_far_inverse = 1.0f / (near_plane - far_plane);

	matrix4_zero(_this);
	_this[0] = tan_fov_half_inverse / aspect_ratio;
	_this[5] = tan_fov_half_inverse;
	_this[10] = (near_plane + far_plane) * near_minus_far_inverse;
	_this[11] = near_plane * far_plane * 2.0f * near_minus_far_inverse;
	_this[14] = -1.0f;
	_this[15] = 1.0f;
}

static inline void matrix4_orthographic(
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
