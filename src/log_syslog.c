#include "log_syslog.h"
#include <syslog.h>

void log_to_syslog(const char *message) {
    openlog("mini-redis", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "%s", message);
    closelog();
}
