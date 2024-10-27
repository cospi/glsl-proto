#include "win32_time.h"

#include <assert.h>

bool win32_performance_frequency(Logger *logger, LONGLONG *out_performance_frequency)
{
	assert(logger != NULL);
	assert(out_performance_frequency != NULL);

	LARGE_INTEGER result;
	if (QueryPerformanceFrequency(&result) == 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting performance frequency failed.");
		return false;
	}

	*out_performance_frequency = result.QuadPart;
	return true;
}

bool win32_performance_counter(Logger *logger, LONGLONG *out_performance_counter)
{
	assert(logger != NULL);
	assert(out_performance_counter != NULL);

	LARGE_INTEGER result;
	if (QueryPerformanceCounter(&result) == 0) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting performance counter failed.");
		return false;
	}

	*out_performance_counter = result.QuadPart;
	return true;
}
