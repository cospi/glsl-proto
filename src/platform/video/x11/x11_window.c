#include "x11_window.h"

#include <assert.h>

Window x11_window_create(
	const X11Connection *connection,
	Logger *logger,
	unsigned int width,
	unsigned int height,
	const char *title
)
{
	assert(connection != NULL);
	assert(logger != NULL);
	assert(width > 0);
	assert(height > 0);
	assert(title != NULL);

	Display *display = connection->display;
	int screen = connection->default_screen;
	Window window = XCreateSimpleWindow(
		display,
		RootWindow(display, screen),
		0,
		0,
		width,
		height,
		0,
		0,
		BlackPixel(display, screen)
	);
	if (window == None) {
		logger->log(logger, LOG_LEVEL_ERROR, "Creating X window failed.");
		return None;
	}

	XStoreName(display, window, title);
	XMapWindow(display, window);

	Atom wm_delete_window = connection->wm_delete_window;
	XSetWMProtocols(display, window, &wm_delete_window, 1);

	logger->log(logger, LOG_LEVEL_INFO, "Created X window (%lu).", window);
	return true;
}
