#include "win32_gl_context_current.h"

#include <assert.h>
#include <inttypes.h>

static bool make_context_current(HDC device_context, HGLRC context, Logger *logger)
{
	assert(logger != NULL);

	if (wglMakeCurrent(device_context, context) == FALSE) {
		logger->log(logger, LOG_LEVEL_ERROR, "Making Win32 OpenGL context current failed.");
		return false;
	}

	logger->log(
		logger,
		LOG_LEVEL_INFO,
		"Made Win32 OpenGL context current (%" PRIxPTR ").",
		(uintptr_t)(void *)context
	);
	return true;
}

bool win32_gl_context_current_init(Win32GlContextCurrent *_this, Logger *logger, const Win32GlContext *context)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(context != NULL);

	return win32_gl_context_current_init_raw(_this, logger, context->window->device_context, context->context);
}

bool win32_gl_context_current_init_raw(
	Win32GlContextCurrent *_this,
	Logger *logger,
	HDC device_context,
	HGLRC context
)
{
	assert(_this != NULL);
	assert(logger != NULL);

	HDC restore_device_context = wglGetCurrentDC();
	HGLRC restore_context = wglGetCurrentContext();

	if (!make_context_current(device_context, context, logger)) {
		return false;
	}

	_this->logger = logger;
	_this->restore_device_context = restore_device_context;
	_this->restore_context = restore_context;
	return true;
}

void win32_gl_context_current_fini(const Win32GlContextCurrent *_this)
{
	assert(_this != NULL);

	make_context_current(_this->restore_device_context, _this->restore_context, _this->logger);
}
