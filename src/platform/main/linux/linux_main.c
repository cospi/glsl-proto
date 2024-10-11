#include <stdlib.h>

#include "../../log/stdlib/stdlib_logger.h"
#include "../../video/x11/x11_connection.h"
#include "../../video/x11/x11_window.h"

static bool handle_events(const X11Connection *connection)
{
	Display *display = connection->display;
	Atom wm_delete_window = connection->wm_delete_window;
	while (XPending(display) > 0) {
		XEvent e;
		XNextEvent(display, &e);
		if ((e.type == ClientMessage) && ((Atom)e.xclient.data.l[0] == wm_delete_window)) {
			return false;
		}
	}

	return true;
}

int main(void)
{
	int exit_status = EXIT_FAILURE;
	StdlibLogger logger;
	X11Connection x11_connection;
	Window window;

	stdlib_logger_init(&logger, stdout, stderr, stderr);

	if (!x11_connection_init(&x11_connection, &logger.base, NULL)) {
		return exit_status;
	}

	window = x11_window_create(&x11_connection, &logger.base, 640, 480, "GLSL Prototyper");
	if (window == None) {
		goto out_x11_connection_fini;
	}

	while (handle_events(&x11_connection)) {
	}

	exit_status = EXIT_SUCCESS;
out_x11_connection_fini:
	x11_connection_fini(&x11_connection);
	return exit_status;
}
