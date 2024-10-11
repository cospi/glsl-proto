#include "x11_gl_context_current.h"

#include <assert.h>
#include <inttypes.h>

static bool make_context_current(Display *display, GLXDrawable drawable, GLXContext context, Logger *logger)
{
	assert(display != NULL);
	assert(logger != NULL);

	if (glXMakeCurrent(display, drawable, context) == False) {
		logger->log(logger, LOG_LEVEL_ERROR, "Making X OpenGL context current failed.");
		return false;
	}

	logger->log(
		logger,
		LOG_LEVEL_INFO,
		"Made X OpenGL context current (%" PRIxPTR ").",
		(uintptr_t)(void *)context
	);
	return true;
}

bool x11_gl_context_current_init(X11GlContextCurrent *_this, Logger *logger, const X11GlContext *context)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(context != NULL);

	GLXDrawable restore_drawable = glXGetCurrentDrawable();
	GLXContext restore_context = glXGetCurrentContext();

	const X11GlWindow *window = context->window;
	Display *display = window->connection->display;
	if (!make_context_current(display, window->window, context->context, logger)) {
		return false;
	}

	_this->logger = logger;
	_this->display = display;
	_this->restore_drawable = restore_drawable;
	_this->restore_context = restore_context;
	return true;
}

void x11_gl_context_current_fini(const X11GlContextCurrent *_this)
{
	assert(_this != NULL);

	make_context_current(_this->display, _this->restore_drawable, _this->restore_context, _this->logger);
}
