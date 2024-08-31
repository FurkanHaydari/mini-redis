#include "log.h"
#include "log_syslog.h"
#include "circular_buffer.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#define MAX_LOG_SIZE 2048
#define BUFFER_SIZE 1024

extern int use_syslog;
static CircularBuffer log_buffer;

void _log_message(const char *level, const char *file, int line, const char *func, const char *format, ...) {
    char formatted_message[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(formatted_message, sizeof(formatted_message), format, args);
    va_end(args);

    char log_message[MAX_LOG_SIZE];
    snprintf(log_message, sizeof(log_message), "[%s] %s:%d:%s: %s\n", level, file, line, func, formatted_message);

    if (use_syslog) {
        buffer_write(&log_buffer, log_message);
    } else {
        printf("%s", log_message);
    }
}

void log_init() {
    if (use_syslog) {
        buffer_init(&log_buffer, BUFFER_SIZE);
    }
}

void log_cleanup() {
    if (use_syslog) {
        log_flush();
        closelog();
    }
}

void log_flush() {
    if (!use_syslog) {
        return; // Do not flush if syslog is not used.
    }

    char log_message[MAX_LOG_SIZE];
    while (1) {
        buffer_read(&log_buffer, log_message, sizeof(log_message) - 1);
        if (strlen(log_message) == 0) {
            break;
        }
        log_to_syslog(log_message);
    }
}
