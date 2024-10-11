#ifndef PLATFORM_VIDEO_X11_X11_CONNECTION_H_
#define PLATFORM_VIDEO_X11_X11_CONNECTION_H_

#include <X11/Xlib.h>

#include <stdbool.h>

#include "../../../log/logger.h"

typedef struct X11Connection {
	Logger *logger;
	Display *display;
	int default_screen;
	Atom wm_delete_window;
} X11Connection;

bool x11_connection_init(X11Connection *_this, Logger *logger, const char *display_name);
void x11_connection_fini(const X11Connection *_this);

#endif // PLATFORM_VIDEO_X11_X11_CONNECTION_H_
