#include "win32_gl_window.h"

#include <assert.h>
#include <inttypes.h>

bool win32_gl_window_init(
	Win32GlWindow *_this,
	Logger *logger,
	UINT style,
	WNDPROC procedure,
	HINSTANCE instance,
	HCURSOR cursor,
	const wchar_t *class_name,
	unsigned int width,
	unsigned int height,
	const wchar_t *title
)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(procedure != NULL);
	assert(class_name != NULL);
	assert(title != NULL);

	WNDCLASSEXW window_class;
	window_class.cbSize = sizeof(WNDCLASSEXW);
	window_class.style = style;
	window_class.lpfnWndProc = procedure;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = instance;
	window_class.hIcon = NULL;
	window_class.hCursor = cursor;
	window_class.hbrBackground = NULL;
	window_class.lpszMenuName = NULL;
	window_class.lpszClassName = class_name;
	window_class.hIconSm = NULL;

	RECT rect;
	HWND window;
	HDC device_context;

	if (RegisterClassExW(&window_class) == 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Registering window class failed.");
		return false;
	}

	rect.left = 0;
	rect.top = 0;
	rect.right = (int)width;
	rect.bottom = (int)height;
	if (AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE) == FALSE) {
		logger->log(logger, LOG_LEVEL_ERROR, "Adjusting window rect failed.");
		goto error_unregister_class_w;
	}

	window = CreateWindowExW(
		0,
		class_name,
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		instance,
		NULL
	);
	if (window == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "Creating window failed.");
		goto error_unregister_class_w;
	}

	device_context = GetDC(window);
	if (device_context == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting device context failed.");
		DestroyWindow(window);
		goto error_unregister_class_w;
	}

	_this->logger = logger;
	_this->instance = instance;
	_this->class_name = class_name;
	_this->window = window;
	_this->device_context = device_context;
	logger->log(logger, LOG_LEVEL_INFO, "Created Win32 OpenGL window (%" PRIxPTR ").", (uintptr_t)(void *)window);
	return true;

error_unregister_class_w:
	UnregisterClassW(class_name, instance);
	return false;
}

void win32_gl_window_fini(const Win32GlWindow *_this)
{
	assert(_this != NULL);

	HWND window = _this->window;
	ReleaseDC(window, _this->device_context);
	DestroyWindow(window);
	UnregisterClassW(_this->class_name, _this->instance);
	Logger *logger = _this->logger;
	logger->log(
		logger,
		LOG_LEVEL_INFO,
		"Destroyed Win32 OpenGL window (%" PRIxPTR ").",
		(uintptr_t)(void *)window
	);
}

void win32_gl_window_show(const Win32GlWindow *_this)
{
	assert(_this != NULL);

	ShowWindow(_this->window, SW_SHOW);
}

void win32_gl_window_swap_buffers(const Win32GlWindow *_this)
{
	assert(_this != NULL);

	SwapBuffers(_this->device_context);
}

void win32_gl_window_get_size(const Win32GlWindow *_this, unsigned int *out_width, unsigned int *out_height)
{
	assert(_this != NULL);
	assert(out_width != NULL);
	assert(out_height != NULL);

	RECT rect;
	if (GetClientRect(_this->window, &rect) == TRUE) {
		*out_width = (unsigned int)(rect.right - rect.left);
		*out_height = (unsigned int)(rect.bottom - rect.top);
	} else {
		*out_width = 0;
		*out_height = 0;
	}
}
