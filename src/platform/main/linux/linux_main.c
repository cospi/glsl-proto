#include <assert.h>
#include <stdlib.h>

#include "../../file_system/executable/linux/linux_executable.h"
#include "../../file_system/stdlib/stdlib_file_system.h"
#include "../../log/stdlib/stdlib_logger.h"
#include "../../memory/stdlib/stdlib_allocator.h"
#include "../../time/posix/posix_time.h"
#include "../../video/x11/x11_input.h"
#include "../../video/x11/x11_setup.h"
#include "../../../proto/proto.h"
#include "../../../time/time.h"

typedef enum HandleEventsResult {
	HANDLE_EVENTS_RESULT_CONTINUE,
	HANDLE_EVENTS_RESULT_EXIT
} HandleEventsResult;

static bool handle_events(const X11Setup *x11_setup, Platform *platform, Proto *proto)
{
	assert(x11_setup != NULL);
	assert(platform != NULL);
	assert(proto != NULL);

	const X11Connection *connection = &x11_setup->connection;
	Display *display = connection->display;
	Atom wm_delete_window = connection->wm_delete_window;

	while (XPending(display) > 0) {
		XEvent e;
		XNextEvent(display, &e);
		switch (e.type) {
		case ClientMessage:
			if ((Atom)e.xclient.data.l[0] == wm_delete_window) {
				return HANDLE_EVENTS_RESULT_EXIT;
			}
			break;
		case ConfigureNotify:
			platform->window_width = (unsigned int)e.xconfigure.width;
			platform->window_height = (unsigned int)e.xconfigure.height;
			break;
		case KeyPress:
			proto_key_press(proto, x11_key_event_to_keycode(&e.xkey));
			break;
		default:
			break;
		}
	}

	return HANDLE_EVENTS_RESULT_CONTINUE;
}

int main(void)
{
	int exit_status = EXIT_FAILURE;
	StdlibLogger logger;
	StdlibAllocator allocator;
	char *executable_directory;
	X11Setup x11_setup;
	StdlibFileSystem file_system;
	Platform platform;
	Proto proto;
	long long previous_time_nsec;

	stdlib_logger_init(&logger, stdout, stderr, stderr);
	stdlib_allocator_init(&allocator, &logger.base);

	executable_directory = linux_get_executable_directory(&logger.base, &allocator.base);
	if (executable_directory == NULL) {
		return exit_status;
	}

	if (!x11_setup_init(&x11_setup, &logger.base, 640, 480, "GLSL Prototyper")) {
		goto out_free_executable_directory;
	}

	stdlib_file_system_init(&file_system, &logger.base, executable_directory);

	x11_gl_window_get_size(&x11_setup.window, &platform.window_width, &platform.window_height);
	platform.logger = &logger.base;
	platform.allocator = &allocator.base;
	platform.file_system = &file_system.base;

	proto_init(&proto, &platform);

	if (!posix_time_nsec(&logger.base, &previous_time_nsec)) {
		goto out_proto_fini;
	}

	while (handle_events(&x11_setup, &platform, &proto) == HANDLE_EVENTS_RESULT_CONTINUE) {
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
out_free_executable_directory:
	allocator.base.free(&allocator.base, executable_directory);
	return exit_status;
}
