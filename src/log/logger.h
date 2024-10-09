#ifndef LOG_LOGGER_H_
#define LOG_LOGGER_H_

typedef enum LogLevel {
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR
} LogLevel;

typedef struct Logger Logger;

struct Logger {
	void (*log)(Logger *_this, LogLevel level, const char *format, ...);
};

#endif // LOG_LOGGER_H_
