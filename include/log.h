#ifndef LOG_H
#define LOG_H

#include "circular_buffer.h"
#include "log_syslog.h"  // For log_to_syslog function

// Log levels
#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2

// Current log level
#define LOG_LEVEL LOG_LEVEL_INFO

// Macros for logging
#if LOG_LEVEL >= LOG_LEVEL_INFO
    #define log_info(format, ...) _log_info(__FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#else
    #define log_info(format, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
    #define log_error(format, ...) _log_error(__FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#else
    #define log_error(format, ...) ((void)0)
#endif

void _log_info(const char *file, int line, const char *func, const char *format, ...);
void _log_error(const char *file, int line, const char *func, const char *format, ...);

void log_init();
void log_cleanup();
void log_flush();

#endif // LOG_H
