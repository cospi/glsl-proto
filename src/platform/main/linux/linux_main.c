#include <stdlib.h>

#include "../../file_system/stdlib/stdlib_file_system.h"
#include "../../log/stdlib/stdlib_logger.h"
#include "../../memory/stdlib/stdlib_allocator.h"
#include "../../video/x11/x11_setup.h"
#include "../../../gl/gl_mesh.h"
#include "../../../gl/gl_program.h"

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
	StdlibFileSystem file_system;
	StdlibAllocator allocator;
	X11Setup x11_setup;
	GlProgram program;
	GlMesh mesh;
	Vertex2 vertices[] = {
		(Vertex2) { (Vector2) { -0.5f, -0.5f }, (Color) { 1.0f, 0.0f, 0.0f, 1.0f } },
		(Vertex2) { (Vector2) { 0.5f, -0.5f }, (Color) { 0.0f, 1.0f, 0.0f, 1.0f } },
		(Vertex2) { (Vector2) { 0.0f, 0.5f }, (Color) { 0.0f, 0.0f, 1.0f, 1.0f } }
	};
	uint16_t indices[] = { 0, 1, 2 };

	stdlib_logger_init(&logger, stdout, stderr, stderr);
	stdlib_file_system_init(&file_system, &logger.base);
	stdlib_allocator_init(&allocator, &logger.base);

	if (!x11_setup_init(&x11_setup, &logger.base, 640, 480, "GLSL Prototyper")) {
		return exit_status;
	}

	if (!gl_program_init_from_files(
		&program,
		&logger.base,
		&allocator.base,
		&file_system.base,
		"res/shaders/test.vert",
		"res/shaders/test.frag"
	)) {
		goto out_x11_setup_fini;
	}

	if (!gl_mesh_init(&mesh, &logger.base, vertices, 3, indices, 3)) {
		goto out_program_fini;
	}

	while (handle_events(&x11_setup.connection)) {
		glClear(GL_COLOR_BUFFER_BIT);
		gl_program_use(&program);
		gl_mesh_render(&mesh);
		x11_gl_window_swap_buffers(&x11_setup.window);
	}

	exit_status = EXIT_SUCCESS;
	gl_mesh_fini(&mesh);
out_program_fini:
	gl_program_fini(&program);
out_x11_setup_fini:
	x11_setup_fini(&x11_setup);
	return exit_status;
}
