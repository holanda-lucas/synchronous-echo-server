#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char *message = "Hello from client.";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("client socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        exit(EXIT_FAILURE);
    } 

    if ((connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }    

    send(sock, message, strlen(message), 0);
    printf("Mensagem enviada.\n");

    int valread = read(sock, buffer, 1024);
    printf("Mensagem recebida pelo servidor: %s\n", buffer);

    close(sock);
    return 0;
}