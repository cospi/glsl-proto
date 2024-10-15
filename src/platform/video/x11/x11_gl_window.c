#include "x11_gl_window.h"

#include <assert.h>

// \note FBConfigs require GLX 1.3 or later.
#define GLX_MAJOR_MIN 1
#define GLX_MINOR_MIN 3

static const int VISUAL_ATTRIBS[] = {
	GLX_X_RENDERABLE, True,
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE, GLX_RGBA_BIT,
	GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DOUBLEBUFFER, True,
	None
};

static bool has_required_glx_version(Display *display)
{
	assert(display != NULL);

	int major, minor;
	return
		(glXQueryVersion(display, &major, &minor) == True)
		&& (
			(major > GLX_MAJOR_MIN)
			|| ((major == GLX_MAJOR_MIN) && (minor >= GLX_MINOR_MIN))
		);
}

static GLXFBConfig find_fb_config(Display *display, int screen)
{
	assert(display != NULL);

	int count;
	GLXFBConfig *fb_configs = glXChooseFBConfig(display, screen, VISUAL_ATTRIBS, &count);
	if (fb_configs == NULL) {
		return NULL;
	}

	GLXFBConfig fb_config = fb_configs[0];
	XFree(fb_configs);
	return fb_config;
}

bool x11_gl_window_init(
	X11GlWindow *_this,
	Logger *logger,
	const X11Connection *connection,
	unsigned int width,
	unsigned int height,
	const char *title
)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(connection != NULL);
	assert(width > 0);
	assert(height > 0);
	assert(title != NULL);

	Display *display = connection->display;
	if (!has_required_glx_version(display)) {
		logger->log(
			logger,
			LOG_LEVEL_ERROR,
			"Required GLX version (minimum %d.%d) not supported.",
			GLX_MAJOR_MIN,
			GLX_MINOR_MIN
		);
		return false;
	}

	int screen = connection->default_screen;
	GLXFBConfig fb_config = find_fb_config(display, screen);
	if (fb_config == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting framebuffer configuration failed.");
		return false;
	}

	XVisualInfo *visual_info = glXGetVisualFromFBConfig(display, fb_config);
	Visual *visual = visual_info->visual;

	Window root_window = RootWindow(display, screen);
	Colormap colormap = XCreateColormap(display, root_window, visual, AllocNone);

	XSetWindowAttributes window_attribs;
	window_attribs.background_pixmap = None;
	window_attribs.border_pixel = 0;
	window_attribs.event_mask = StructureNotifyMask | KeyPressMask;
	window_attribs.colormap = colormap;

	Window window = XCreateWindow(
		display,
		root_window,
		0,
		0,
		width,
		height,
		0,
		visual_info->depth,
		InputOutput,
		visual,
		CWBorderPixel | CWColormap | CWEventMask,
		&window_attribs
	);

	XFree(visual_info);

	if (window == None) {
		XFreeColormap(display, colormap);
		logger->log(logger, LOG_LEVEL_ERROR, "Creating X OpenGL window failed.");
		return false;
	}

	XStoreName(display, window, title);
	XMapWindow(display, window);

	Atom wm_delete_window = connection->wm_delete_window;
	XSetWMProtocols(display, window, &wm_delete_window, 1);

	_this->logger = logger;
	_this->connection = connection;
	_this->fb_config = fb_config;
	_this->colormap = colormap;
	_this->window = window;
	logger->log(logger, LOG_LEVEL_INFO, "Created X OpenGL window (%lu).", window);
	return true;
}

void x11_gl_window_fini(const X11GlWindow *_this)
{
	assert(_this != NULL);

	Window window = _this->window;
	Display *display = _this->connection->display;
	XDestroyWindow(display, window);
	XFreeColormap(display, _this->colormap);
	Logger *logger = _this->logger;
	logger->log(logger, LOG_LEVEL_INFO, "Closed X OpenGL window (%lu).", window);
}

void x11_gl_window_swap_buffers(const X11GlWindow *_this)
{
	assert(_this != NULL);

	glXSwapBuffers(_this->connection->display, _this->window);
}

void x11_gl_window_get_size(const X11GlWindow *_this, unsigned int *out_width, unsigned int *out_height)
{
	assert(_this != NULL);
	assert(out_width != NULL);
	assert(out_height != NULL);

	XWindowAttributes window_attribs;
	XGetWindowAttributes(_this->connection->display, _this->window, &window_attribs);
	*out_width = (unsigned int)window_attribs.width;
	*out_height = (unsigned int)window_attribs.height;
}
