#ifndef RENDER_SPRITE_H_
#define RENDER_SPRITE_H_

#include "../math/rect2.h"

typedef struct Sprite {
	Rect2 rect;
	Rect2 uv;
} Sprite;

#endif // RENDER_SPRITE_H_
