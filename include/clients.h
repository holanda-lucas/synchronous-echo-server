#ifndef CLIENTS
#define CLIENTS

#include <stdlib.h>
#include <poll.h>

void init_clients(struct pollfd *fds, size_t size);

void print_new_client(int socket, char* IP, int PORT);

void add_new_client(struct pollfd *fds, size_t size, int new_socket);

#endif