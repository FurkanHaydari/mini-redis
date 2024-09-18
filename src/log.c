#include "log.h"
#include "log_syslog.h"
#include "circular_buffer.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <time.h>

#define MAX_LOG_SIZE 2048
#define BUFFER_SIZE 1024

extern int use_syslog;
static CircularBuffer log_buffer;
int current_log_level = LOG_LEVEL_ERROR;

void set_log_level(int level)
{
    current_log_level = level;
}

void _log_message(const char *level, const char *file, int line, const char *func, const char *format, ...)
{
    va_list args;
    char message[1024];
    char timestamp[20];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    char full_message[2048];
    snprintf(full_message, sizeof(full_message), "[%s] %s %s:%d %s(): %s\n",
             timestamp, level, file, line, func, message);

    if (use_syslog)
    {
        syslog(LOG_INFO, "%s", full_message);
    }
    else
    {
        fprintf(stderr, "%s", full_message);
    }
}

void log_init()
{
    if (use_syslog)
    {
        buffer_init(&log_buffer, BUFFER_SIZE);
    }
}

void log_cleanup()
{
    if (use_syslog)
    {
        log_flush();
        closelog();
    }
}

void log_flush()
{
    if (!use_syslog)
    {
        return; // Do not flush if syslog is not used.
    }

    char log_message[MAX_LOG_SIZE];
    while (1)
    {
        buffer_read(&log_buffer, log_message, sizeof(log_message) - 1);
        if (strlen(log_message) == 0)
        {
            break;
        }
        log_to_syslog(log_message);
    }
}
