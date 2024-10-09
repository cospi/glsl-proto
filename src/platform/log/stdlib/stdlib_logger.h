#ifndef PLATFORM_LOG_STDLIB_STDLIB_LOGGER_H_
#define PLATFORM_LOG_STDLIB_STDLIB_LOGGER_H_

#include <stdio.h>

#include "../../../log/logger.h"

typedef struct StdlibLogger {
	Logger base;
	FILE *info_stream;
	FILE *warning_stream;
	FILE *error_stream;
} StdlibLogger;

void stdlib_logger_init(StdlibLogger *_this, FILE *info_stream, FILE *warning_stream, FILE *error_stream);

#endif // PLATFORM_LOG_STDLIB_STDLIB_LOGGER_H_
