#include <stdlib.h>

#include "../../file_system/stdlib/stdlib_file_system.h"
#include "../../log/stdlib/stdlib_logger.h"
#include "../../memory/stdlib/stdlib_allocator.h"
#include "../../video/x11/x11_setup.h"
#include "../../../proto/proto.h"

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
	stdlib_logger_init(&logger, stdout, stderr, stderr);

	X11Setup x11_setup;
	if (!x11_setup_init(&x11_setup, &logger.base, 640, 480, "GLSL Prototyper")) {
		return EXIT_FAILURE;
	}

	StdlibFileSystem file_system;
	stdlib_file_system_init(&file_system, &logger.base);

	StdlibAllocator allocator;
	stdlib_allocator_init(&allocator, &logger.base);

	Platform platform;
	platform.logger = &logger.base;
	platform.allocator = &allocator.base;
	platform.file_system = &file_system.base;

	Proto proto;
	proto_init(&proto, &platform);

	while (handle_events(&x11_setup.connection)) {
		proto_tick(&proto);
		x11_gl_window_swap_buffers(&x11_setup.window);
	}

	proto_fini(&proto);
	x11_setup_fini(&x11_setup);
	return EXIT_SUCCESS;
}
