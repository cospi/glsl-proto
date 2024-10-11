#include <stdlib.h>

#include "../../log/stdlib/stdlib_logger.h"
#include "../../video/x11/x11_connection.h"

int main(void)
{
	StdlibLogger logger;
	X11Connection x11_connection;

	stdlib_logger_init(&logger, stdout, stderr, stderr);

	if (!x11_connection_init(&x11_connection, &logger.base, NULL)) {
		return EXIT_FAILURE;
	}

	x11_connection_fini(&x11_connection);
	return EXIT_SUCCESS;
}
