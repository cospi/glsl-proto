#include "win32_gl_context.h"

#include <assert.h>
#include <inttypes.h>

#include "win32_gl_extensions.h"
#include "../../../gl/gl.h"

static const int PIXEL_FORMAT_ATTRIBS[] = {
	WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
	WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
	WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
	WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
	WGL_COLOR_BITS_ARB, 32,
	WGL_DEPTH_BITS_ARB, 24,
	WGL_ALPHA_BITS_ARB, 8,
	0
};
static const int CONTEXT_ATTRIBS[] = {
	WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
	WGL_CONTEXT_MINOR_VERSION_ARB, 0,
	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	0
};

bool win32_gl_context_init(Win32GlContext *_this, Logger *logger, const Win32GlWindow *window)
{
	assert(_this != NULL);
	assert(logger != NULL);
	assert(window != NULL);

	HDC device_context = window->device_context;
	int suggested_pixel_format_index;
	UINT pixel_format_count;
	if (wglChoosePixelFormatARB(
		device_context,
		PIXEL_FORMAT_ATTRIBS,
		NULL,
		1,
		&suggested_pixel_format_index,
		&pixel_format_count
	) == FALSE) {
		logger->log(logger, LOG_LEVEL_ERROR, "Choosing pixel format failed.");
		return false;
	}

	PIXELFORMATDESCRIPTOR pixel_format;
	pixel_format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixel_format.nVersion = 1;
	pixel_format.dwFlags = PFD_SUPPORT_OPENGL;
	pixel_format.iPixelType = PFD_TYPE_RGBA;
	pixel_format.cColorBits = 32;
	pixel_format.cRedBits = 0;
	pixel_format.cRedShift = 0;
	pixel_format.cGreenBits = 0;
	pixel_format.cGreenShift = 0;
	pixel_format.cBlueBits = 0;
	pixel_format.cBlueShift = 0;
	pixel_format.cAlphaBits = 8;
	pixel_format.cAlphaShift = 0;
	pixel_format.cAccumBits = 0;
	pixel_format.cAccumRedBits = 0;
	pixel_format.cAccumGreenBits = 0;
	pixel_format.cAccumBlueBits = 0;
	pixel_format.cAccumAlphaBits = 0;
	pixel_format.cDepthBits = 24;
	pixel_format.cStencilBits = 0;
	pixel_format.cAuxBuffers = 0;
	pixel_format.iLayerType = 0;
	pixel_format.bReserved = 0;
	pixel_format.dwLayerMask = 0;
	pixel_format.dwVisibleMask = 0;
	pixel_format.dwDamageMask = 0;

	if (DescribePixelFormat(
		device_context,
		suggested_pixel_format_index,
		sizeof(PIXELFORMATDESCRIPTOR),
		&pixel_format
	) == 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Describing pixel format failed.");
		return false;
	}

	if (SetPixelFormat(device_context, suggested_pixel_format_index, &pixel_format) == FALSE) {
		logger->log(logger, LOG_LEVEL_ERROR, "Setting pixel format failed.");
		return false;
	}

	HGLRC context = wglCreateContextAttribsARB(device_context, NULL, CONTEXT_ATTRIBS);
	if (context == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "Creating Win32 OpenGL context failed.");
		return false;
	}

	_this->logger = logger;
	_this->window = window;
	_this->context = context;
	logger->log(
		logger,
		LOG_LEVEL_INFO,
		"Created Win32 OpenGL context (%" PRIxPTR ").",
		(uintptr_t)(void *)context
	);
	return true;
}

void win32_gl_context_fini(const Win32GlContext *_this)
{
	assert(_this != NULL);

	HGLRC context = _this->context;
	wglDeleteContext(context);
	Logger *logger = _this->logger;
	logger->log(
		logger,
		LOG_LEVEL_INFO,
		"Destroyed Win32 OpenGL context (%" PRIxPTR ").",
		(uintptr_t)(void *)context
	);
}
