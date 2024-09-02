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


int use_syslog = 0;

void init();
void cleanup();

void parse_arguments(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "s")) != -1) {
        switch (opt) {
            case 's':
                use_syslog = 1; // Use syslog
                break;
            default:
                fprintf(stderr, "Usage: %s [-s]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

extern int server_socket;
int main(int argc, char *argv[]) {
    // Initialize logging
    parse_arguments(argc, argv);
    
    init();

    if (use_syslog) {
        openlog("mini-redis", LOG_PID | LOG_CONS, LOG_USER);
        fprintf(stderr, "Logging directed to syslog.");
    } else {
        fprintf(stderr, "Logging directed to the console.");
    }
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    server_socket = start_server(PORT);
    if (server_socket < 0) {
        log_error("Server could not be started.");
        return EXIT_FAILURE;
    }

    log_info("Starting...");
    accept_connections();  // Accept connections
    cleanup();

    return EXIT_SUCCESS;
}
