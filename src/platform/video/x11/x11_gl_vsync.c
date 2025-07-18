#include "x11_gl_vsync.h"

#include <assert.h>

#include "x11_gl_extensions.h"

void x11_gl_set_vsync(const X11GlWindow *window, bool vsync)
{
	assert(window != NULL);

	const X11Connection *connection = window->connection;
	Display *display = connection->display;
	if (x11_gl_has_extension(
		glXQueryExtensionsString(display, connection->default_screen),
		"GLX_EXT_swap_control"
	)) {
		PFNGLXSWAPINTERVALEXTPROC glx_swap_interval_ext =
			(PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const GLubyte *)"glXSwapIntervalEXT");
		if (glx_swap_interval_ext != NULL) {
			glx_swap_interval_ext(display, window->window, vsync ? 1 : 0);
		}
	}
}
