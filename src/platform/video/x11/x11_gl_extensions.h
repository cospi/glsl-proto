#ifndef PLATFORM_VIDEO_X11_X11_GL_EXTENSIONS_H_
#define PLATFORM_VIDEO_X11_X11_GL_EXTENSIONS_H_

#include <stdbool.h>

#include "../../../log/logger.h"

bool x11_gl_has_extension(const char *extensions, const char *extension);
bool x11_gl_init_extensions(Logger *logger);

#endif // PLATFORM_VIDEO_X11_X11_GL_EXTENSIONS_H_
