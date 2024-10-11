#ifndef PLATFORM_VIDEO_X11_X11_WINDOW_H_
#define PLATFORM_VIDEO_X11_X11_WINDOW_H_

#include "x11_connection.h"

Window x11_window_create(
	const X11Connection *connection,
	Logger *logger,
	unsigned int width,
	unsigned int height,
	const char *title
);

#endif // PLATFORM_VIDEO_X11_X11_WINDOW_H_
