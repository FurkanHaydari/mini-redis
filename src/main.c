#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "server.h"
#include "log.h"
#include <stdbool.h>
#include "database.h"
#include "config.h"
#include <getopt.h>

int use_syslog = 0;
int port = PORT;
int log_level = LOG_LEVEL_ERROR;

void init();
void cleanup();

void parse_arguments(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "p:is")) != -1)
    {
        switch (opt)
        {
        case 'p':
            port = atoi(optarg);
            break;
        case 'i':
            log_level = LOG_LEVEL_INFO;
            break;
        case 's':
            use_syslog = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-p port] [-i] [-s]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

extern int server_socket;

int main(int argc, char *argv[])
{
    printf("Starting Mini-Redis Server\n");
    printf("Log Level: %s\n", log_level == LOG_LEVEL_INFO ? "INFO" : "ERROR");
    printf("Logging Destination: %s\n", use_syslog ? "Syslog" : "Console");
    printf("Port: %d\n", port);
    printf("----------------------------------------\n");
    parse_arguments(argc, argv);

    init();

    set_log_level(log_level);

    if (use_syslog)
    {
        openlog("mini-redis", LOG_PID | LOG_CONS, LOG_USER);
        log_info("Logging directed to syslog.");
    }
    else
    {
        log_info("Logging directed to the console.");
    }

    log_info("Starting server on port %d", port);

    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (start_server(port) == -1)
    {
        log_error("Failed to start server");
        cleanup();
        return 1;
    }

    accept_connections();

    cleanup();
    return 0;
}
