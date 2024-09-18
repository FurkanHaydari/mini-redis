#ifndef LOG_H
#define LOG_H

#include "circular_buffer.h"
#include "log_syslog.h" // For log_to_syslog function
#include <stdarg.h>

// Log levels
#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2

extern int current_log_level;

// Macros for logging
#if current_log_level >= LOG_LEVEL_INFO
#define log_info(format, ...) _log_message("INFO", __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#else
#define log_info(format, ...) ((void)0)
#endif

#if current_log_level >= LOG_LEVEL_ERROR
#define log_error(format, ...) _log_message("ERROR", __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#else
#define log_error(format, ...) ((void)0)
#endif
void log_init();
void log_cleanup();
void log_flush();
void set_log_level(int level);
void _log_message(const char *level, const char *file, int line, const char *func, const char *format, ...);

#endif // LOG_H
