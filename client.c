//
// Created by Filip on 4. 1. 2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 5555

int sock;

void* receive_thread(void* arg) {
    char buffer[256];

    while (1) {
        ssize_t len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;

        buffer[len] = '\0';
        printf("Server: %s\n", buffer);
    }

    return NULL;
}

void run_client() {
    sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Connected to server.\n");

    pthread_t tid;
    pthread_create(&tid, NULL, receive_thread, NULL);

    char msg[256];

    while (1) {
        fgets(msg, sizeof(msg), stdin);
        send(sock, msg, strlen(msg), 0);
    }

    close(sock);
}

int main() {
    run_client();
    return 0;
}
