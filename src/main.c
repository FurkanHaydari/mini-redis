// main.c - Entry point for the Mini-Redis server
// This file contains the main function and initialization logic

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

// Global configuration variables
int use_syslog = 0;              // Flag to determine if syslog should be used for logging
int port = PORT;                 // Port number for the server to listen on
int log_level = LOG_LEVEL_ERROR; // Current log level

// Function prototypes
void init();
void cleanup();

// Parse command-line arguments
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
    // Print startup information
    printf("Starting Mini-Redis Server\n");
    printf("Log Level: %s\n", log_level == LOG_LEVEL_INFO ? "INFO" : "ERROR");
    printf("Logging Destination: %s\n", use_syslog ? "Syslog" : "Console");
    printf("Port: %d\n", port);
    printf("----------------------------------------\n");

    parse_arguments(argc, argv);

    init();

    set_log_level(log_level);

    // Configure logging destination
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

    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Start the server
    if (start_server(port) == -1)
    {
        log_error("Failed to start server");
        cleanup();
        return 1;
    }

    // Main server loop
    accept_connections();

    cleanup();
    return 0;
}
