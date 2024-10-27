#ifndef PLATFORM_VIDEO_WIN32_WIN32_SETUP_H_
#define PLATFORM_VIDEO_WIN32_WIN32_SETUP_H_

#include "win32_gl_context_current.h"

typedef struct Win32Setup {
	Win32GlWindow window;
	Win32GlContext gl_context;
	Win32GlContextCurrent gl_context_current;
} Win32Setup;

bool win32_setup_init(
	Win32Setup *_this,
	Logger *logger,
	WNDPROC procedure,
	HINSTANCE instance,
	const wchar_t *window_class_name,
	unsigned int width,
	unsigned int height,
	const wchar_t *title
);
void win32_setup_fini(const Win32Setup *_this);

#endif // PLATFORM_VIDEO_WIN32_WIN32_SETUP_H_
