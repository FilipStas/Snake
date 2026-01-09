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
        if (len <= 0) return NULL;
        buffer[len] = '\0';
        printf("%s\n", buffer);
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
        //msg[strcspn(msg, "\r\n")] = 0; // odstráni \n

        if (strcmp(msg, "quit\n") == 0) {
            printf("Exiting client...\n");
            close(sock);  // zatvoríme socket → receive_thread skončí
            break;        // opustí while loop
        }

        send(sock, msg, strlen(msg), 0);
    }
    exit(0);
}
int main() {
    run_client();
    return 0;
}
