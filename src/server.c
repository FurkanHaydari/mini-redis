#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h> // For errno
#include "server.h"
#include "log.h"
#include "database.h"

#define MAX_PORT_TRIES 10
#define BUFFER_SIZE 1024

int server_socket = -1;

void init(){
    db_init();
    log_init();
}

void cleanup() {
    if (server_socket != -1) {
        close(server_socket);
    }
    db_cleanup();
    log_cleanup();  // Clean up logging
    exit(EXIT_SUCCESS);
}

void signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        log_info("Signal received, shutting down...");
        cleanup();
    }
}

int start_server(int base_port) {
    int server_socket;
    struct sockaddr_in server_addr;
    int port = base_port;
    int port_found = 0;

    while (port < base_port + MAX_PORT_TRIES) {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            log_error("Failed to create socket: %s", strerror(errno));
            return -1;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        // Log port number
        log_info("Trying port: %d", port);
        fprintf(stderr,"Actual port: %d\n",port);

        if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
            port_found = 1;
            break;
        } else {
            log_error("Bind error: %s", strerror(errno));
        }

        close(server_socket);
        port++;
    }

    if (!port_found) {
        log_error("No suitable port found.");
        return -1;
    }

    if (listen(server_socket, 10) == -1) {
        log_error("Listen error: %s", strerror(errno));
        close(server_socket);
        return -1;
    }

    log_info("Server started. Port: %d", port);
    return server_socket;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    log_info("Client connected: socket %d", client_socket);

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';  // Add null terminator

        log_info("Received command: %s", buffer);

        if (strncmp(buffer, "SET ", 4) == 0) {
            // Handle SET command
            char *key = strtok(buffer + 4, " ");
            char *value = strtok(NULL, "\r\n");
            if (key && value) {
                if (db_set(key, value) == 0) {
                    send(client_socket, "OK\n", 3, 0);
                    log_info("SET command successful for key: %s and value: %s", key, value);
                } else {
                    send(client_socket, "ERROR\n", 6, 0);
                    log_error("SET command failed for key: %s and value: %s", key, value);
                }
            } else {
                send(client_socket, "ERROR\n", 6, 0);
                log_error("SET command malformed");
            }
        } else if (strncmp(buffer, "GET ", 4) == 0) {
            // Handle GET command
            char *key = strtok(buffer + 4, "\r\n");
            char *value = db_get(key);
            if (value) {
                send(client_socket, value, strlen(value), 0);
                send(client_socket, "\n", 1, 0);
                log_info("GET command successful for key: %s and value: %s", key, value);
            } else {
                send(client_socket, "Not Found\n", 10, 0);
                log_info("GET command: key not found %s", key);
            }
        } else if (strncmp(buffer, "DEL ", 4) == 0) {
            // Handle DEL command
            char *key = strtok(buffer + 4, "\r\n");
            if (db_delete(key) == 0) {
                send(client_socket, "Deleted\n", 8, 0);
                log_info("DEL command successful for key: %s", key);
            } else {
                send(client_socket, "ERROR\n", 6, 0);
                log_error("DEL command failed for key: %s", key);
            }
        } else {
            send(client_socket, "ERROR: Unsupported command\n", 27, 0);
            log_error("Unsupported command received: %s", buffer);
        }
    }

    if (bytes_received < 0) {
        log_error("Error receiving data from socket %d", client_socket);
    }

    log_info("Client disconnected: socket %d", client_socket);
    close(client_socket);
}

void accept_connections() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket;

    while (1) {
        // Break the loop if server_socket is invalid
        if (server_socket < 0) {
            log_error("Invalid server_socket. Connections cannot be accepted.");
            break;
        }

        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            log_error("Accept error: %s", strerror(errno));
            // Exit loop on critical error
            break;
        }
        // const char *client_ip = inet_ntoa(client_addr.sin_addr);
        // fprintf(stderr,"LAN: %s",client_ip);
        // log_info("New connection: %s", client_ip);
        handle_client(client_socket);
        close(client_socket);
    }
    log_info("Server is shutting down, performing clean-up...");
    cleanup();
}
