#ifndef MATH_RECT2_H_
#define MATH_RECT2_H_

#include "vector2.h"

typedef struct Rect2 {
	Vector2 position;
	Vector2 size;
} Rect2;

static inline void rect2_min_max(const Rect2 *rect, Vector2 *out_min, Vector2 *out_max)
{
	assert(rect != NULL);
	assert(out_min != NULL);
	assert(out_max != NULL);

	*out_min = rect->position;
	*out_max = vector2_sum(&rect->position, &rect->size);
}

#endif // MATH_RECT2_H_
