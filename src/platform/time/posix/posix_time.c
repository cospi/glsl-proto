#include "posix_time.h"

#include <time.h>

#include <assert.h>
#include <stddef.h>

#include "../../../time/time.h"

bool posix_time_nsec(Logger *logger, long long *out_time_nsec)
{
	assert(logger != NULL);
	assert(out_time_nsec != NULL);

	struct timespec time;
	if (clock_gettime(CLOCK_MONOTONIC, &time) == -1) {
		logger->log(logger, LOG_LEVEL_ERROR, "Getting time failed.");
		return false;
	}

	*out_time_nsec = (time.tv_sec * NSEC_PER_SEC) + time.tv_nsec;
	return true;
}
