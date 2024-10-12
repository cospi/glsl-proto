#include <GL/gl.h>

#include <stdlib.h>

#include "../../file_system/stdlib/stdlib_file_system.h"
#include "../../log/stdlib/stdlib_logger.h"
#include "../../video/x11/x11_setup.h"

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
	StdlibLogger logger;
	StdlibFileSystem file_system;
	FileHandle file;
	X11Setup x11_setup;

	stdlib_logger_init(&logger, stdout, stderr, stderr);
	stdlib_file_system_init(&file_system, &logger.base);

	file = file_system.base.open_file(&file_system.base, "Makefile");
	if (file != NULL) {
		size_t size;
		if (file_system.base.try_get_file_size(&file_system.base, file, &size)) {
			logger.base.log(&logger.base, LOG_LEVEL_INFO, "%zu", size);
		}
		file_system.base.close_file(&file_system.base, file);
	}

	if (!x11_setup_init(&x11_setup, &logger.base, 640, 480, "GLSL Prototyper")) {
		return EXIT_FAILURE;
	}

	while (handle_events(&x11_setup.connection)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.5f, -0.5f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.5f, -0.5f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.0f, 0.5f);
		glEnd();
		x11_gl_window_swap_buffers(&x11_setup.window);
	}

	x11_setup_fini(&x11_setup);
	return EXIT_SUCCESS;
}
