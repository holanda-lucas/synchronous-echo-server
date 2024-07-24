#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <poll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "clients.h"

#define MAX_CLIENTS 10
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    int opt;
    struct pollfd fds[MAX_CLIENTS + 1]; // +1 porque o índice zero é o fd do servidor   

    init_clients(&fds, MAX_CLIENTS);

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("server socket");
        exit(EXIT_FAILURE);
    }

    opt = 1;
    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) < 0) {
        perror("sock opt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&address, addrlen) < 0) {
        perror("server bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("server listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        int activity = poll(fds, MAX_CLIENTS + 1, -1);

        if (activity < 0) {
            perror("poll");
            close(new_socket);
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Verificação de atividade no servidor
        if (fds[0].revents & POLLIN) {
            if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                close(new_socket);
                close(server_fd);
                exit(EXIT_FAILURE);
            }
            print_new_client(new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            add_new_client(&fds, MAX_CLIENTS + 1, new_socket);
        }

        // Verificação de atividade nos clientes
        for (int i = 1; i < MAX_CLIENTS + 1; i++) {
            if (fds[i].revents & POLLIN) {
                int valread = read(fds[i].fd, buffer, BUFFER_SIZE);

                if (valread == 0) {
                    // Cliente desconectou
                    getpeername(fds[i].fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    printf("Can't read data: Disconnected client [IP: %s PORT: %d]\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(fds[i].fd);
                    fds[i].fd = -1;
                } else {
                    // Valor lido
                    buffer[valread] = '\0';
                    printf("Mensagem recebida: %s\n", buffer);
                    send(fds[i].fd, buffer, strlen(buffer) + 1, 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}