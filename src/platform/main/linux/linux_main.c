#include <GL/gl.h>

#include <stdlib.h>

#include "../../log/stdlib/stdlib_logger.h"
#include "../../video/x11/x11_connection.h"
#include "../../video/x11/x11_gl_context_current.h"

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
	X11GlWindow window;
	X11GlContext gl_context;
	X11GlContextCurrent gl_context_current;

	stdlib_logger_init(&logger, stdout, stderr, stderr);

	if (!x11_connection_init(&x11_connection, &logger.base, NULL)) {
		return exit_status;
	}

	if (!x11_gl_window_init(&window, &logger.base, &x11_connection, 640, 480, "GLSL Prototyper")) {
		goto out_x11_connection_fini;
	}

	if (!x11_gl_context_init(&gl_context, &logger.base, &window)) {
		goto out_x11_gl_window_fini;
	}

	if (!x11_gl_context_current_init(&gl_context_current, &logger.base, &gl_context)) {
		goto out_x11_gl_context_fini;
	}

	while (handle_events(&x11_connection)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.5f, -0.5f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.5f, -0.5f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.0f, 0.5f);
		glEnd();
		x11_gl_window_swap_buffers(&window);
	}

	exit_status = EXIT_SUCCESS;
	x11_gl_context_current_fini(&gl_context_current);
out_x11_gl_context_fini:
	x11_gl_context_fini(&gl_context);
out_x11_gl_window_fini:
	x11_gl_window_fini(&window);
out_x11_connection_fini:
	x11_connection_fini(&x11_connection);
	return exit_status;
}
