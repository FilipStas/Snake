#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <termios.h>
#include "server.h"

#define SERVER_BIN "./server"
#define MENU_MESS "Start [new], Quit [quit]\n"

static int client_socket = -1;
static struct termios orig_term;

static void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}

static void set_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &orig_term) == 0) {
        atexit(restore_terminal);
        struct termios t = orig_term;
        t.c_lflag &= ~(ICANON | ECHO);
        t.c_cc[VMIN] = 1;
        t.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }
}

static int try_connect_once() {
    struct sockaddr_in server_addr = {0};
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) return -1;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) return 0;
    close(client_socket); client_socket = -1; return -1;
}

static int connect_with_retry(int attempts, int delay_ms) {
    for (int i = 0; i < attempts; ++i) {
        if (try_connect_once() == 0) return 0;
        usleep(delay_ms * 1000);
    }
    return -1;
}

static int start_server() {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return -1; }
    if (pid == 0) {
        execl(SERVER_BIN, SERVER_BIN, NULL);
        perror("execl");
        _exit(1);
    }
    return 0;
}

static int ensure_server_and_connect() {
    if (connect_with_retry(5, 200)== 0) {
        return 0;
    }
    if (start_server() != 0){
        return -1;
    }
    return connect_with_retry(30, 150);
}

static void* receive_thread(void* arg) {
    char buffer[4096];
    while (1) {
        ssize_t recv_len = recv(client_socket, buffer, sizeof(buffer)-1, 0);
        if (recv_len <= 0) {
            printf("\nDisconnected from server.\n");
            break;
        }
        buffer[recv_len] = 0;
        printf("%s", buffer);
        fflush(stdout);
    }
    return NULL;
}

static int read_arrow_direction(char *out_dir) {
    unsigned char ch1, ch2;
    if (read(STDIN_FILENO, &ch1, 1) <= 0) return 0;
    if (ch1 != '[') return 0;

    unsigned char seq_buf[8];
    int seq_len = 0;
    while (seq_len < (int)sizeof(seq_buf)) {
        if (read(STDIN_FILENO, &ch2, 1) <= 0) return 0;
        seq_buf[seq_len++] = ch2;
        if (ch2 == 'A' || ch2 == 'B' || ch2 == 'C' || ch2 == 'D') {
            if (ch2 == 'A') *out_dir = 'U';
            else if (ch2 == 'B') *out_dir = 'D';
            else if (ch2 == 'C') *out_dir = 'R';
            else if (ch2 == 'D') *out_dir = 'L';
            return 1;
        }
        if ((ch2 >= 'a' && ch2 <= 'z') || (ch2 >= 'A' && ch2 <= 'Z')) break;
    }
    return 0;
}

static void play_loop() {
    pthread_t recv_thread_id;
    int recv_started = 0;
    if (pthread_create(&recv_thread_id, NULL, receive_thread, NULL) == 0) {
        recv_started = 1;
    } else {
        perror("pthread_create");
    }
    set_raw_mode();

    while (1) {
        unsigned char ch;
        if (read(STDIN_FILENO, &ch, 1) <= 0) break;
        if (ch == 0x1b) {
            char dir = 0;
            if (read_arrow_direction(&dir)) {
                send(client_socket, &dir, 1, 0);
            }
        } else {
            if (ch == 'q' || ch == 'Q') { char q='q'; send(client_socket,&q,1,0); break; }
            if (ch == 'r' || ch == 'R') { char r='r'; send(client_socket,&r,1,0); }
        }
    }
    if (recv_started) {
        shutdown(client_socket, SHUT_RDWR);
        pthread_join(recv_thread_id, NULL);
    }
    restore_terminal();
    close(client_socket);
}

int main() {
    printf("Hello!\n");
    printf(MENU_MESS);

    char cmd[64];
    while (fgets(cmd, sizeof(cmd), stdin)) {
        cmd[strcspn(cmd, "\r\n")] = 0;
        if (!strcmp(cmd, "help")) { printf(MENU_MESS); continue; }
        if (!strcmp(cmd, "quit")) { printf("Bye.\n"); break; }
        if (!strcmp(cmd, "new")) {
            if (ensure_server_and_connect() != 0) {
                fprintf(stderr, "Failed to start/connect to server.\n");
                continue;
            }
            printf("Connected.\nUse arrow keys. Press r to restart, q to quit.\n");
            play_loop();
            printf("Client closed.\n");
        } else {
            printf("Unknown command.\n");
        }
        printf(MENU_MESS);
    }
    return 0;
}