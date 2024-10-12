#include <stdlib.h>

#include "../../file_system/stdlib/stdlib_file_system.h"
#include "../../log/stdlib/stdlib_logger.h"
#include "../../memory/stdlib/stdlib_allocator.h"
#include "../../time/posix/posix_time.h"
#include "../../video/x11/x11_setup.h"
#include "../../../proto/proto.h"
#include "../../../time/time.h"

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
	X11Setup x11_setup;
	StdlibFileSystem file_system;
	StdlibAllocator allocator;
	Platform platform;
	Proto proto;
	long long previous_time_nsec;

	stdlib_logger_init(&logger, stdout, stderr, stderr);

	if (!x11_setup_init(&x11_setup, &logger.base, 640, 480, "GLSL Prototyper")) {
		return exit_status;
	}

	stdlib_file_system_init(&file_system, &logger.base);
	stdlib_allocator_init(&allocator, &logger.base);

	platform.logger = &logger.base;
	platform.allocator = &allocator.base;
	platform.file_system = &file_system.base;

	proto_init(&proto, &platform);

	if (!posix_time_nsec(&logger.base, &previous_time_nsec)) {
		goto out_proto_fini;
	}

	while (handle_events(&x11_setup.connection)) {
		long long time_nsec;
		if (!posix_time_nsec(&logger.base, &time_nsec)) {
			goto out_proto_fini;
		}

		proto_tick(&proto, (float)((long double)(time_nsec - previous_time_nsec) / NSEC_PER_SEC));
		x11_gl_window_swap_buffers(&x11_setup.window);

		previous_time_nsec = time_nsec;
	}

	exit_status = EXIT_SUCCESS;
out_proto_fini:
	proto_fini(&proto);
	x11_setup_fini(&x11_setup);
	return exit_status;
}
