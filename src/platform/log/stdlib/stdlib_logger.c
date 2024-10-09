#include "stdlib_logger.h"

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>

static const char *LOG_LEVEL_NAMES[] = {
	[LOG_LEVEL_INFO] = "Info",
	[LOG_LEVEL_WARNING] = "Warning",
	[LOG_LEVEL_ERROR] = "Error"
};

static FILE *stdlib_logger_get_log_level_stream(const StdlibLogger *_this, LogLevel level)
{
	switch (level) {
	case LOG_LEVEL_INFO:
		return _this->info_stream;
	case LOG_LEVEL_WARNING:
		return _this->warning_stream;
	case LOG_LEVEL_ERROR:
		return _this->error_stream;
	default:
		assert(false);
		return NULL;
	}
}

static void stdlib_logger_log(Logger *_this, LogLevel level, const char *format, ...)
{
	assert(_this != NULL);
	assert(((uintptr_t)(void *)_this % _Alignof(StdlibLogger)) == 0);
	assert(format != NULL);

	FILE *stream = stdlib_logger_get_log_level_stream((const StdlibLogger *)_this, level);
	if (stream == NULL) {
		return;
	}

	fputc('[', stream);
	fputs(LOG_LEVEL_NAMES[level], stream);
	fputs("] ", stream);

	va_list args;
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);

	fputc('\n', stream);
	fflush(stream);
}

void stdlib_logger_init(StdlibLogger *_this, FILE *info_stream, FILE *warning_stream, FILE *error_stream)
{
	assert(_this != NULL);

	_this->base.log = stdlib_logger_log;
	_this->info_stream = info_stream;
	_this->warning_stream = warning_stream;
	_this->error_stream = error_stream;
}
