#include <stdio.h>

// Declare the functions from server.c and client.c
void run_server();
void run_client();

int main() {
    // Example logic to decide between server and client
    int mode = 0; // 0 for server, 1 for client
    printf("Enter mode (0 for server, 1 for client): ");
    scanf("%d", &mode);

    if (mode == 0) {
        run_server();
    } else {
        run_client();
    }

    return 0;
}