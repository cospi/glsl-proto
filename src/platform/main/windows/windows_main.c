#include <stdlib.h>

#include "../../file_system/executable/windows/windows_executable.h"
#include "../../file_system/stdlib/stdlib_file_system.h"
#include "../../log/stdlib/stdlib_logger.h"
#include "../../memory/stdlib/stdlib_allocator.h"
#include "../../time/win32/win32_time.h"
#include "../../video/win32/win32_input.h"
#include "../../video/win32/win32_setup.h"
#include "../../../platform/platform.h"
#include "../../../proto/proto.h"

static Platform *s_platform = NULL;
static Proto *s_proto = NULL;
static bool s_running = true;

typedef enum HandleEventsResult {
	HANDLE_EVENTS_RESULT_CONTINUE,
	HANDLE_EVENTS_RESULT_EXIT
} HandleEventsResult;

static HandleEventsResult handle_events(void)
{
	for (MSG message; PeekMessageW(&message, NULL, 0, 0, PM_REMOVE) != 0;) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	return s_running ? HANDLE_EVENTS_RESULT_CONTINUE : HANDLE_EVENTS_RESULT_EXIT;
}

static LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	switch (message) {
	case WM_SIZE:
		if (s_platform != NULL) {
			s_platform->window_width = (unsigned int)LOWORD(l_param);
			s_platform->window_height = (unsigned int)HIWORD(l_param);
		}
		break;
	case WM_CLOSE:
		s_running = false;
		return 0;
	case WM_KEYDOWN:
		if (s_proto != NULL) {
			proto_key_press(s_proto, win32_key_to_keycode(w_param));
		}
		break;
	default:
		break;
	}

	return DefWindowProcW(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance, PSTR command_line, int command_show)
{
	(void)previous_instance;
	(void)command_line;
	(void)command_show;

	int exit_status = EXIT_FAILURE;
	StdlibLogger logger;
	StdlibAllocator allocator;
	char *executable_directory;
	Win32Setup win32_setup;
	StdlibFileSystem file_system;
	Platform platform;
	Proto proto;
	LONGLONG performance_frequency;
	LONGLONG previous_performance_counter;

	stdlib_logger_init(&logger, stdout, stderr, stderr);
	stdlib_allocator_init(&allocator, &logger.base);

	executable_directory = windows_get_executable_directory(&logger.base, &allocator.base);
	if (executable_directory == NULL) {
		return exit_status;
	}

	logger.base.log(&logger.base, LOG_LEVEL_INFO, executable_directory);

	if (!win32_setup_init(
		&win32_setup,
		&logger.base,
		window_procedure,
		instance,
		L"GLSLPrototyperWindowClass",
		640,
		480,
		L"GLSL Prototyper"
	)) {
		goto out_free_executable_directory;
	}

	stdlib_file_system_init(&file_system, &logger.base, executable_directory);

	win32_gl_window_get_size(&win32_setup.window, &platform.window_width, &platform.window_height);
	platform.logger = &logger.base;
	platform.allocator = &allocator.base;
	platform.file_system = &file_system.base;

	s_platform = &platform;
	s_proto = &proto;

	proto_init(&proto, &platform);

	if (!win32_performance_frequency(&logger.base, &performance_frequency)) {
		goto out_fini_proto;
	}

	if (!win32_performance_counter(&logger.base, &previous_performance_counter)) {
		goto out_fini_proto;
	}

	while (handle_events() == HANDLE_EVENTS_RESULT_CONTINUE) {
		LONGLONG performance_counter;
		LONGLONG delta_performance_counter;

		if (!win32_performance_counter(&logger.base, &performance_counter)) {
			goto out_fini_proto;
		}

		delta_performance_counter = performance_counter - previous_performance_counter;

		proto_tick(&proto, (float)((long double)delta_performance_counter / performance_frequency));
		win32_gl_window_swap_buffers(&win32_setup.window);

		previous_performance_counter = performance_counter;
	}

	exit_status = EXIT_SUCCESS;
out_fini_proto:
	proto_fini(&proto);
	win32_setup_fini(&win32_setup);
out_free_executable_directory:
	allocator.base.free(&allocator.base, executable_directory);
	return exit_status;
}
