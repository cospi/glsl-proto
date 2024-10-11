#ifndef PLATFORM_VIDEO_X11_X11_SETUP_H_
#define PLATFORM_VIDEO_X11_X11_SETUP_H_

#include "x11_gl_context_current.h"

typedef struct X11Setup {
	X11Connection connection;
	X11GlWindow window;
	X11GlContext gl_context;
	X11GlContextCurrent gl_context_current;
} X11Setup;

bool x11_setup_init(X11Setup *_this, Logger *logger, unsigned int width, unsigned int height, const char *title);
void x11_setup_fini(const X11Setup *_this);

#endif // PLATFORM_VIDEO_X11_X11_SETUP_H_
