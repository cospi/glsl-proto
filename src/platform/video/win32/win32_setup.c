#include "win32_setup.h"

#include <assert.h>

#include "win32_gl_extensions.h"

bool win32_setup_init(
	Win32Setup *_this,
	Logger *logger,
	WNDPROC procedure,
	HINSTANCE instance,
	const wchar_t *window_class_name,
	unsigned int width,
	unsigned int height,
	const wchar_t *title
)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(procedure != NULL);
	assert(window_class_name != NULL);
	assert(width > 0);
	assert(height > 0);
	assert(title != NULL);

	Win32GlWindow *window;
	Win32GlContext *gl_context;
	Win32GlContextCurrent *gl_context_current;

	if (!win32_gl_init_context_creation_extensions(logger, instance)) {
		return false;
	}

	window = &_this->window;
	if (!win32_gl_window_init(
		window,
		logger,
		CS_VREDRAW | CS_HREDRAW | CS_OWNDC,
		procedure,
		instance,
		LoadCursor(NULL, IDC_ARROW),
		window_class_name,
		width,
		height,
		title
	)) {
		return false;
	}

	gl_context = &_this->gl_context;
	if (!win32_gl_context_init(gl_context, logger, window)) {
		goto error_window_fini;
	}

	gl_context_current = &_this->gl_context_current;
	if (!win32_gl_context_current_init(gl_context_current, logger, gl_context)) {
		goto error_gl_context_fini;
	}

	if (!win32_gl_init_extensions(logger)) {
		goto error_gl_context_current_fini;
	}

	win32_gl_window_show(window);
	return true;

error_gl_context_current_fini:
	win32_gl_context_current_fini(gl_context_current);
error_gl_context_fini:
	win32_gl_context_fini(gl_context);
error_window_fini:
	win32_gl_window_fini(window);
	return false;
}

void win32_setup_fini(const Win32Setup *_this)
{
	assert(_this != NULL);

	win32_gl_context_current_fini(&_this->gl_context_current);
	win32_gl_context_fini(&_this->gl_context);
	win32_gl_window_fini(&_this->window);
}
