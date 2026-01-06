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

void* client_thread(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);

    char buffer[256];

    while (1) {
        ssize_t len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        buffer[len] = '\0';
        printf("Client says: %s\n", buffer);

        // echo back
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    return NULL;
}

void run_server() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int* client_socket = malloc(sizeof(int));
        *client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, client_socket);
        pthread_detach(tid);
    }

    close(server_socket);
}

int main() {
    run_server();
    return 0;
}
