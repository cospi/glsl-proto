#ifndef MATH_VECTOR3_H_
#define MATH_VECTOR3_H_

#include <assert.h>
#include <stddef.h>
#include <math.h>

typedef struct Vector3 {
	float x;
	float y;
	float z;
} Vector3;

static inline Vector3 vector3_sum(const Vector3 *left, const Vector3 *right)
{
	assert(left != NULL);
	assert(right != NULL);

	return (Vector3) { left->x + right->x, left->y + right->y, left->z + right->z };
}

static inline float vector3_dot(const Vector3 *left, const Vector3 *right)
{
	assert(left != NULL);
	assert(right != NULL);

	return (left->x * right->x) + (left->y * right->y) + (left->z * right->z);
}

static inline float vector3_length(const Vector3 *_this)
{
	assert(_this != NULL);

	return sqrtf(vector3_dot(_this, _this));
}

static inline Vector3 vector3_normalized(Vector3 vector)
{
	float inverse_length = 1.0f / vector3_length(&vector);
	return (Vector3) { vector.x * inverse_length, vector.y * inverse_length, vector.z * inverse_length };
}

#endif // MATH_VECTOR3_H_
