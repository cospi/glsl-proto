#include "x11_connection.h"

#include <assert.h>
#include <inttypes.h>

bool x11_connection_init(X11Connection *_this, Logger *logger, const char *display_name)
{
	assert(_this != NULL);
	assert(logger != NULL);

	Display *display = XOpenDisplay(display_name);
	if (display == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "Opening connection to X server failed.");
		return false;
	}

	_this->logger = logger;
	_this->display = display;
	_this->default_screen = DefaultScreen(display);
	_this->wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	logger->log(
		logger,
		LOG_LEVEL_INFO,
		"Opened connection to X server (%" PRIxPTR ").",
		(uintptr_t)(void *)display
	);
	return true;
}

void x11_connection_fini(const X11Connection *_this)
{
	assert(_this != NULL);

	Display *display = _this->display;
	XCloseDisplay(display);
	Logger *logger = _this->logger;
	logger->log(
		logger,
		LOG_LEVEL_INFO,
		"Closed connection to X server (%" PRIxPTR ").",
		(uintptr_t)(void *)display
	);
}
