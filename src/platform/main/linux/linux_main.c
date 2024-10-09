#include <stdlib.h>

#include "../../log/stdlib/stdlib_logger.h"

int main(void)
{
	StdlibLogger logger;
	stdlib_logger_init(&logger, stdout, stderr, stderr);

	logger.base.log(&logger.base, LOG_LEVEL_INFO, "Test %d", 1234);
	logger.base.log(&logger.base, LOG_LEVEL_WARNING, "Test");
	logger.base.log(&logger.base, LOG_LEVEL_ERROR, "Test");

	return EXIT_SUCCESS;
}
