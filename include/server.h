#ifndef SERVER_H
#define SERVER_H

int start_server(int port);
void signal_handler(int signum);
void handle_client(int client_socket);
void accept_connections();
#endif