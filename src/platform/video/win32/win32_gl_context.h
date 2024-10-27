#ifndef PLATFORM_VIDEO_WIN32_WIN32_GL_CONTEXT_H_
#define PLATFORM_VIDEO_WIN32_WIN32_GL_CONTEXT_H_

#include "win32_gl_window.h"

typedef struct Win32GlContext {
	Logger *logger;
	const Win32GlWindow *window;
	HGLRC context;
} Win32GlContext;

bool win32_gl_context_init(Win32GlContext *_this, Logger *logger, const Win32GlWindow *window);
void win32_gl_context_fini(const Win32GlContext *_this);

#endif // PLATFORM_VIDEO_WIN32_WIN32_GL_CONTEXT_H_
