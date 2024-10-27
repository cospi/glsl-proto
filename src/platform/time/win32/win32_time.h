#ifndef PLATFORM_TIME_WIN32_WIN32_TIME_H_
#define PLATFORM_TIME_WIN32_WIN32_TIME_H_

#include <Windows.h>

#include <stdbool.h>

#include "../../../log/logger.h"

bool win32_performance_frequency(Logger *logger, LONGLONG *out_performance_frequency);
bool win32_performance_counter(Logger *logger, LONGLONG *out_performance_counter);

#endif // PLATFORM_TIME_WIN32_WIN32_TIME_H_
