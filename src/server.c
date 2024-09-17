#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <json-c/json.h>
#include "server.h"
#include "log.h"
#include "database.h"
#include "config.h"

int server_socket = -1;

void init()
{
    db_init();
    log_init();
}

void cleanup()
{
    if (server_socket != -1)
    {
        close(server_socket);
    }
    db_cleanup();
    log_cleanup(); // Clean up logging
    exit(EXIT_SUCCESS);
}

void signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM)
    {
        log_info("Signal received, shutting down...");
        cleanup();
    }
}

int start_server(int base_port)
{
    struct sockaddr_in server_addr;
    int port = base_port;
    int port_found = 0;

    while (port < base_port + MAX_PORT_TRIES)
    {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1)
        {
            log_error("Failed to create socket: %s", strerror(errno));
            return -1;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        // Log port number
        log_info("Trying port: %d", port);
        fprintf(stderr, "Actual port: %d\n", port);

        if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        {
            port_found = 1;
            break;
        }
        else
        {
            log_error("Bind error: %s", strerror(errno));
        }

        close(server_socket);
        port++;
    }

    if (!port_found)
    {
        log_error("No suitable port found.");
        return -1;
    }

    if (listen(server_socket, 10) == -1)
    {
        log_error("Listen error: %s", strerror(errno));
        close(server_socket);
        return -1;
    }

    log_info("Server started. Port: %d", port);
    return server_socket;
}

void handle_set_command(int client_socket, const char *key, const char *value)
{
    // Create a JSON object from the value string
    json_object *json_value = json_object_new_string(value);
    if (db_set(key, json_value) == 0)
    {
        send(client_socket, "OK\n", 3, 0);
        log_info("SET command successful for key: %s and value: %s", key, value);
    }
    else
    {
        send(client_socket, "ERROR\n", 6, 0);
        log_error("SET command failed for key: %s and value: %s", key, value);
    }
}

void handle_get_command(int client_socket, const char *key)
{
    json_object *value_obj = db_get(key);
    if (value_obj)
    {
        const char *value_str = json_object_to_json_string(value_obj); // Convert JSON object to string
        send(client_socket, value_str, strlen(value_str), 0);
        send(client_socket, "\n", 1, 0); // Optional: send newline character for better formatting
        log_info("GET command successful for key: %s and value: %s", key, value_str);
    }
    else
    {
        send(client_socket, "Not Found\n", 10, 0);
        log_info("GET command: key not found %s", key);
    }
}

void handle_del_command(int client_socket, const char *key)
{
    if (db_delete(key) == 0)
    {
        send(client_socket, "Deleted\n", 8, 0);
        log_info("DEL command successful for key: %s", key);
    }
    else
    {
        send(client_socket, "ERROR\n", 6, 0);
        log_error("DEL command failed for key: %s", key);
    }
}

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0'; // Null terminate the string
        log_info("Received data: %s\n", buffer);

        // Parse JSON
        struct json_object *parsed_json;
        struct json_object *key_obj;
        struct json_object *operation_obj;
        struct json_object *value_obj;

        parsed_json = json_tokener_parse(buffer);
        if (parsed_json == NULL)
        {
            log_error("Failed to parse JSON\n");
            send(client_socket, "ERROR: Invalid JSON", 19, 0);
            close(client_socket);
            return;
        }

        json_object_object_get_ex(parsed_json, "key", &key_obj);
        json_object_object_get_ex(parsed_json, "operation", &operation_obj);
        json_object_object_get_ex(parsed_json, "value", &value_obj);

        const char *key_str = json_object_get_string(key_obj);
        const char *op_str = json_object_get_string(operation_obj);

        if (key_str == NULL || op_str == NULL)
        {
            log_error("Key or operation missing in JSON\n");
            send(client_socket, "ERROR: Key or operation missing", 31, 0);
            close(client_socket);
            json_object_put(parsed_json);
            return;
        }

        // Handle operation
        if (strcmp(op_str, "GET") == 0)
        {
            handle_get_command(client_socket, key_str);
        }
        else if (strcmp(op_str, "DEL") == 0)
        {
            handle_del_command(client_socket, key_str);
        }
        else if (strcmp(op_str, "SET") == 0)
        {

            json_object_object_get_ex(parsed_json, "value", &value_obj);
            const char *value_str = json_object_get_string(value_obj);
            handle_set_command(client_socket, key_str, value_str);
        }
        else
        {
            log_error("Unknown operation\n");
            send(client_socket, "ERROR: Unknown operation", 25, 0);
        }
        json_object_put(parsed_json);
    }
    else
    {
        log_error("Failed to receive data\n");
        send(client_socket, "ERROR: Failed to receive data", 30, 0);
    }
    close(client_socket);
}

void accept_connections()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket;

    while (1)
    {
        // Break the loop if server_socket is invalid
        if (server_socket < 0)
        {
            log_error("Invalid server_socket. Connections cannot be accepted.");
            break;
        }

        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0)
        {
            log_error("Client accept failed: %s", strerror(errno));
            continue;
        }
        handle_client(client_socket);
    }
    close(client_socket);
    log_info("Server is shutting down, performing clean-up...");
    cleanup();
}
