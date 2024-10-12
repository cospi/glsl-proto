#ifndef PLATFORM_VIDEO_X11_X11_GL_WINDOW_H_
#define PLATFORM_VIDEO_X11_X11_GL_WINDOW_H_

#include <GL/glx.h>

#include "x11_connection.h"

typedef struct X11GlWindow {
	Logger *logger;
	const X11Connection *connection;
	GLXFBConfig fb_config;
	Colormap colormap;
	Window window;
} X11GlWindow;

bool x11_gl_window_init(
	X11GlWindow *_this,
	Logger *logger,
	const X11Connection *connection,
	unsigned int width,
	unsigned int height,
	const char *title
);
void x11_gl_window_fini(const X11GlWindow *_this);
void x11_gl_window_swap_buffers(const X11GlWindow *_this);
void x11_gl_window_get_size(const X11GlWindow *_this, unsigned int *out_width, unsigned int *out_height);

#endif // PLATFORM_VIDEO_X11_X11_GL_WINDOW_H_
