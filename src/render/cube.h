#ifndef RENDER_CUBE_H_
#define RENDER_CUBE_H_

#include <stdint.h>

#include "vertex3.h"

#define CUBE_VERTEX_COUNT 24
#define CUBE_INDEX_COUNT 36

void cube_init(Vertex3 *vertices, uint16_t *indices);

#endif // RENDER_CUBE_H_
