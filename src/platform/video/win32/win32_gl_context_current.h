#ifndef PLATFORM_VIDEO_WIN32_WIN32_GL_CONTEXT_CURRENT_H_
#define PLATFORM_VIDEO_WIN32_WIN32_GL_CONTEXT_CURRENT_H_

#include "win32_gl_context.h"

typedef struct Win32GlContextCurrent {
	Logger *logger;
	HDC restore_device_context;
	HGLRC restore_context;
} Win32GlContextCurrent;

bool win32_gl_context_current_init(Win32GlContextCurrent *_this, Logger *logger, const Win32GlContext *context);
bool win32_gl_context_current_init_raw(
	Win32GlContextCurrent *_this,
	Logger *logger,
	HDC device_context,
	HGLRC context
);
void win32_gl_context_current_fini(const Win32GlContextCurrent *_this);

#endif // PLATFORM_VIDEO_WIN32_WIN32_GL_CONTEXT_CURRENT_H_
