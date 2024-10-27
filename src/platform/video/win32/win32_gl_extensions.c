#include "win32_gl_extensions.h"

#include <assert.h>

#include "win32_gl_context_current.h"
#include "../../../gl/gl.h"

static const wchar_t DUMMY_WINDOW_TITLE[] = L"DummyWindow";
static const wchar_t DUMMY_WINDOW_CLASS_NAME[] = L"DummyWindowClass";

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

static HGLRC create_dummy_context(HDC device_context)
{
	assert(device_context != NULL);

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

	int suggested_pixel_format_index = ChoosePixelFormat(device_context, &pixel_format);
	if (suggested_pixel_format_index == 0) {
		return NULL;
	}

	if (DescribePixelFormat(
		device_context,
		suggested_pixel_format_index,
		sizeof(PIXELFORMATDESCRIPTOR),
		&pixel_format
	) == 0) {
		return NULL;
	}

	if (SetPixelFormat(device_context, suggested_pixel_format_index, &pixel_format) == FALSE) {
		return NULL;
	}

	return wglCreateContext(device_context);
}

bool win32_gl_init_context_creation_extensions(Logger *logger, HINSTANCE instance)
{
	bool exit_status = false;
	Win32GlWindow dummy_window;
	HGLRC dummy_context;
	Win32GlContextCurrent dummy_gl_context_current;
	PFNWGLCHOOSEPIXELFORMATARBPROC wgl_choose_pixel_format_arb;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wgl_create_context_attribs_arb;

	if (!win32_gl_window_init(
		&dummy_window,
		logger,
		CS_OWNDC,
		DefWindowProcW,
		instance,
		NULL,
		DUMMY_WINDOW_CLASS_NAME,
		0,
		0,
		DUMMY_WINDOW_TITLE
	)) {
		return exit_status;
	}

	dummy_context = create_dummy_context(dummy_window.device_context);
	if (dummy_context == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "Creating Win32 OpenGL dummy context failed.");
		goto out_dummy_window_fini;
	}

	if (!win32_gl_context_current_init_raw(
		&dummy_gl_context_current,
		logger,
		dummy_window.device_context,
		dummy_context
	)) {
		goto out_delete_dummy_context;
	}

	// \note wglGetProcAddress requires nasty casting.
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wcast-function-type"

	wgl_choose_pixel_format_arb = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (wgl_choose_pixel_format_arb == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "wglChoosePixelFormatARB not found.");
		goto out_dummy_gl_context_current_fini;
	}

	wgl_create_context_attribs_arb =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wgl_create_context_attribs_arb == NULL) {
		logger->log(logger, LOG_LEVEL_ERROR, "wglCreateContextAttribsARB not found.");
		goto out_dummy_gl_context_current_fini;
	}

	#pragma GCC diagnostic pop

	wglChoosePixelFormatARB = wgl_choose_pixel_format_arb;
	wglCreateContextAttribsARB = wgl_create_context_attribs_arb;

	exit_status = true;
out_dummy_gl_context_current_fini:
	win32_gl_context_current_fini(&dummy_gl_context_current);
out_delete_dummy_context:
	wglDeleteContext(dummy_context);
out_dummy_window_fini:
	win32_gl_window_fini(&dummy_window);
	return exit_status;
}

bool win32_gl_init_extensions(Logger *logger)
{
	assert(logger != NULL);

	// \note wglGetProcAddress requires nasty casting.
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wcast-function-type"

	#define INIT_PROC(type, proc) \
		do { \
			proc = (type)wglGetProcAddress(#proc); \
			if (proc == NULL) { \
				logger->log(logger, LOG_LEVEL_ERROR, "Getting OpenGL extension " #proc " failed."); \
				return false; \
			} \
		} while (false)

	INIT_PROC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
	INIT_PROC(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
	INIT_PROC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);

	INIT_PROC(PFNGLGENBUFFERSPROC, glGenBuffers);
	INIT_PROC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
	INIT_PROC(PFNGLBINDBUFFERPROC, glBindBuffer);
	INIT_PROC(PFNGLBUFFERDATAPROC, glBufferData);
	INIT_PROC(PFNGLMAPBUFFERPROC, glMapBuffer);
	INIT_PROC(PFNGLUNMAPBUFFERPROC, glUnmapBuffer);

	INIT_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
	INIT_PROC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);

	INIT_PROC(PFNGLCREATESHADERPROC, glCreateShader);
	INIT_PROC(PFNGLDELETESHADERPROC, glDeleteShader);
	INIT_PROC(PFNGLSHADERSOURCEPROC, glShaderSource);
	INIT_PROC(PFNGLCOMPILESHADERPROC, glCompileShader);
	INIT_PROC(PFNGLGETSHADERIVPROC, glGetShaderiv);
	INIT_PROC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);

	INIT_PROC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
	INIT_PROC(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
	INIT_PROC(PFNGLATTACHSHADERPROC, glAttachShader);
	INIT_PROC(PFNGLDETACHSHADERPROC, glDetachShader);
	INIT_PROC(PFNGLLINKPROGRAMPROC, glLinkProgram);
	INIT_PROC(PFNGLUSEPROGRAMPROC, glUseProgram);
	INIT_PROC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
	INIT_PROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
	INIT_PROC(PFNGLUSEPROGRAMPROC, glUseProgram);

	INIT_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
	INIT_PROC(PFNGLUNIFORM1FPROC, glUniform1f);
	INIT_PROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);

	#undef INIT_PROC

	#pragma GCC diagnostic pop

	return true;
}
