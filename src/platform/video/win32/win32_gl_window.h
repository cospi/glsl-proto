#ifndef PLATFORM_VIDEO_WIN32_WIN32_GL_WINDOW_H_
#define PLATFORM_VIDEO_WIN32_WIN32_GL_WINDOW_H_

#include <stdbool.h>

#include <Windows.h>

#include "../../../log/logger.h"

typedef struct Win32GlWindow {
	Logger *logger;
	HINSTANCE instance;
	const wchar_t *class_name;
	HWND window;
	HDC device_context;
} Win32GlWindow;

bool win32_gl_window_init(
	Win32GlWindow *_this,
	Logger *logger,
	UINT style,
	WNDPROC procedure,
	HINSTANCE instance,
	HCURSOR cursor,
	const wchar_t *class_name,
	unsigned int width,
	unsigned int height,
	const wchar_t *title
);
void win32_gl_window_fini(const Win32GlWindow *_this);
void win32_gl_window_show(const Win32GlWindow *_this);
void win32_gl_window_swap_buffers(const Win32GlWindow *_this);
void win32_gl_window_get_size(const Win32GlWindow *_this, unsigned int *out_width, unsigned int *out_height);

#endif // PLATFORM_VIDEO_WIN32_WIN32_GL_WINDOW_H_
