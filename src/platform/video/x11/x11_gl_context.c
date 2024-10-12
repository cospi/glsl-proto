#include "x11_gl_context.h"

#include <assert.h>
#include <inttypes.h>

#include "x11_gl_extensions.h"

static const int CONTEXT_ATTRIBS[] = {
	GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
	GLX_CONTEXT_MINOR_VERSION_ARB, 0,
	GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
	None
};

static bool s_error = false;

static int signal_error(Display *display, XErrorEvent *error_event)
{
	(void)display;
	(void)error_event;

	s_error = true;
	return 0;
}

bool x11_gl_context_init(X11GlContext *_this, Logger *logger, const X11GlWindow *window)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(window != NULL);

	PFNGLXCREATECONTEXTATTRIBSARBPROC glx_create_context_attribs_arb =
		(PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
	if (glx_create_context_attribs_arb == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "glXCreateContextAttribsARB not found.");
		return false;
	}

	const X11Connection *connection = window->connection;
	Display *display = connection->display;
	int screen = connection->default_screen;
	const char *extensions = glXQueryExtensionsString(display, screen);
	if (!x11_gl_has_extension(extensions, "GLX_ARB_create_context")) {
		logger->log(logger, LOG_LEVEL_ERROR, "GLX_ARB_create_context not found.");
		return false;
	}

	int (*previous_error_handler)(Display *, XErrorEvent *) = XSetErrorHandler(signal_error);
	GLXContext context = glx_create_context_attribs_arb(display, window->fb_config, NULL, True, CONTEXT_ATTRIBS);
	XSync(display, False);
	XSetErrorHandler(previous_error_handler);
	if ((context == NULL) || s_error) {
		s_error = false;
		if (context != NULL) {
			glXDestroyContext(display, context);
		}
		logger->log(logger, LOG_LEVEL_ERROR, "Creating X OpenGL context failed.");
		return false;
	}

	_this->logger = logger;
	_this->window = window;
	_this->context = context;
	logger->log(logger, LOG_LEVEL_INFO, "Created X OpenGL context (%" PRIxPTR ").", (uintptr_t)(void *)context);
	return true;
}

void x11_gl_context_fini(const X11GlContext *_this)
{
	assert(_this != NULL);

	GLXContext context = _this->context;
	glXDestroyContext(_this->window->connection->display, context);
	Logger *logger = _this->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Destroyed X OpenGL context (%" PRIxPTR ").", (uintptr_t)(void *)context);
}
