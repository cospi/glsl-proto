#ifndef GL_GL_TEXTURE_H_
#define GL_GL_TEXTURE_H_

#include "gl.h"
#include "../render/image.h"

typedef struct GlTexture {
	Logger *logger;
	GLuint texture;
} GlTexture;

bool gl_texture_init_from_image(GlTexture *_this, Logger *logger, const Image *image);
bool gl_texture_init_from_tga_file(
	GlTexture *_this,
	Logger *logger,
	Allocator *allocator,
	FileSystem *file_system,
	const char *path
);
void gl_texture_fini(const GlTexture *_this);
void gl_texture_bind(const GlTexture *_this);

#endif // GL_GL_TEXTURE_H_
