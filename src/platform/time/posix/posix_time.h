#ifndef PLATFORM_TIME_POSIX_POSIX_TIME_H_
#define PLATFORM_TIME_POSIX_POSIX_TIME_H_

#include <stdbool.h>

#include "../../../log/logger.h"

bool posix_time_nsec(Logger *logger, long long *out_time_nsec);

#endif // PLATFORM_TIME_POSIX_POSIX_TIME_H_
