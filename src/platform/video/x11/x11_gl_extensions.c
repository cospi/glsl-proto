#include "x11_gl_extensions.h"

#include <assert.h>
#include <string.h>

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
