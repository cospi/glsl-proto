#include "x11_setup.h"

#include <assert.h>

#include "x11_gl_extensions.h"

bool x11_setup_init(X11Setup *_this, Logger *logger, unsigned int width, unsigned int height, const char *title)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(width > 0);
	assert(height > 0);
	assert(title != NULL);

	X11Connection *connection;
	X11GlWindow *window;
	X11GlContext *gl_context;
	X11GlContextCurrent *gl_context_current;

	connection = &_this->connection;
	if (!x11_connection_init(connection, logger, NULL)) {
		return false;
	}

	window = &_this->window;
	if (!x11_gl_window_init(window, logger, connection, width, height, title)) {
		goto error_fini_connection;
	}

	gl_context = &_this->gl_context;
	if (!x11_gl_context_init(gl_context, logger, window)) {
		goto error_fini_window;
	}

	gl_context_current = &_this->gl_context_current;
	if (!x11_gl_context_current_init(gl_context_current, logger, gl_context)) {
		goto error_fini_gl_context;
	}

	if (!x11_gl_init_extensions(logger)) {
		goto error_fini_gl_context_current;
	}

	return true;

error_fini_gl_context_current:
	x11_gl_context_current_fini(gl_context_current);
error_fini_gl_context:
	x11_gl_context_fini(gl_context);
error_fini_window:
	x11_gl_window_fini(window);
error_fini_connection:
	x11_connection_fini(connection);
	return false;
}

void x11_setup_fini(const X11Setup *_this)
{
	assert(_this != NULL);

	x11_gl_context_current_fini(&_this->gl_context_current);
	x11_gl_context_fini(&_this->gl_context);
	x11_gl_window_fini(&_this->window);
	x11_connection_fini(&_this->connection);
}
