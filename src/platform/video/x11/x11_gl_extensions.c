#include "x11_gl_extensions.h"

#include <GL/glx.h>

#include <assert.h>
#include <string.h>

#include "../../../gl/gl.h"

bool x11_gl_has_extension(const char *extensions, const char *extension)
{
	assert(extensions != NULL);
	assert(extension != NULL);

	if (*extensions == '\0') {
		return false;
	}

	size_t extension_length = strlen(extension);
	for (const char *start = extensions;;) {
		const char *found = strstr(start, extension);
		if (found == NULL) {
			break;
		}

		const char *remaining = found + extension_length;
		if ((found == start) || (found[-1] == ' ')) {
			char terminator = *remaining;
			if ((terminator == '\0') || (terminator == ' ')) {
				return true;
			}
		}

		start = remaining;
	}

	return false;
}

bool x11_gl_init_extensions(Logger *logger)
{
	assert(logger != NULL);

	#define INIT_PROC(type, proc) \
		do { \
			proc = (type)glXGetProcAddressARB((const GLubyte *)#proc); \
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
	INIT_PROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);

	#undef INIT_PROC

	return true;
}
