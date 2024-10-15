#ifndef MATH_VECTOR2_H_
#define MATH_VECTOR2_H_

#include <assert.h>
#include <stddef.h>

typedef struct Vector2 {
	float x;
	float y;
} Vector2;

static inline Vector2 vector2_sum(const Vector2 *left, const Vector2 *right)
{
	assert(left != NULL);
	assert(right != NULL);

	return (Vector2) { left->x + right->x, left->y + right->y };
}

static inline Vector2 vector2_difference(const Vector2 *left, const Vector2 *right)
{
	assert(left != NULL);
	assert(right != NULL);

	return (Vector2) { left->x - right->x, left->y - right->y };
}

static inline Vector2 vector2_scale(const Vector2 *left, float right)
{
	assert(left != NULL);

	return (Vector2) { left->x * right, left->y * right };
}

#endif // MATH_VECTOR2_H_
