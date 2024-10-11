#ifndef PLATFORM_VIDEO_X11_X11_GL_CONTEXT_H_
#define PLATFORM_VIDEO_X11_X11_GL_CONTEXT_H_

#include "x11_gl_window.h"

typedef struct X11GlContext {
	Logger *logger;
	const X11GlWindow *window;
	GLXContext context;
} X11GlContext;

bool x11_gl_context_init(X11GlContext *_this, Logger *logger, const X11GlWindow *window);
void x11_gl_context_fini(const X11GlContext *_this);

#endif // PLATFORM_VIDEO_X11_X11_GL_CONTEXT_H_
