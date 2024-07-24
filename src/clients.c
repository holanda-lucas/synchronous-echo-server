#include "clients.h"

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>

void init_clients(struct pollfd *fds, size_t size) {
    for (int i = 0; i < size; i++) {
        fds[i].fd = -1;
    }
}

void print_new_client(int socket, char* IP, int PORT) {
    printf("New Connection: \n");
            printf("\t Socket FD: %d\n", socket);
            printf("\t IP: %s\n", IP);
            printf("\t PORT: %d\n", PORT);
}

void add_new_client(struct pollfd *fds, size_t size, int new_socket) {
    for(int i = 1; i < size; i++) {
        if (fds[i].fd == -1) {
            fds[i].fd = new_socket;
            fds[i].events = POLLIN;
            break;
        }
    }
}