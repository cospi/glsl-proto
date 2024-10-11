#ifndef PLATFORM_VIDEO_X11_X11_GL_CONTEXT_CURRENT_H_
#define PLATFORM_VIDEO_X11_X11_GL_CONTEXT_CURRENT_H_

#include "x11_gl_context.h"

typedef struct X11GlContextCurrent {
	Logger *logger;
	Display *display;
	GLXDrawable restore_drawable;
	GLXContext restore_context;
} X11GlContextCurrent;

bool x11_gl_context_current_init(X11GlContextCurrent *_this, Logger *logger, const X11GlContext *context);
void x11_gl_context_current_fini(const X11GlContextCurrent *_this);

#endif // PLATFORM_VIDEO_X11_X11_GL_CONTEXT_CURRENT_H_
