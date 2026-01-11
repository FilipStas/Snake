#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/select.h>
#include <string.h>
#include "world.h"
#include "server.h"

#define TICK 120000

static int active_clients = 0;
static pthread_mutex_t clients_mtx;
static pthread_cond_t clients_cond;
static int listen_fd = -1;
static int shutdown_requested = 0;

static void send_frame(int client_fd, const World *world, const char *extra) {
    char buf[4096];
    char grid[HEIGHT][WIDTH];

    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
            grid[y][x] = ' ';

    if (world->fruit.x >= 0 && world->fruit.y >= 0)
        grid[world->fruit.y][world->fruit.x] = '$';

    for (int i = 0; i < world->len; ++i) {
        char ch = (i == 0) ? '#' : 'o';
        grid[world->snake[i].y][world->snake[i].x] = ch;
    }

    int p = 0;
    p += snprintf(buf + p, sizeof(buf) - p, "\x1b[H\x1b[2J");
    p += snprintf(buf + p, sizeof(buf) - p, "Score: %d\n", world->score);

    for (int y = 0; y < HEIGHT; ++y) {
        p += snprintf(buf + p, sizeof(buf) - p, "|");
        for (int x = 0; x < WIDTH; ++x)
            p += snprintf(buf + p, sizeof(buf) - p, "%c", grid[y][x]);
        p += snprintf(buf + p, sizeof(buf) - p, "|\n");
    }

    if (extra)
        p += snprintf(buf + p, sizeof(buf) - p, "%s\n", extra);

    p += snprintf(buf + p, sizeof(buf) - p, "(Move with arrows, r=restart, q=quit)\n");

    send(client_fd, buf, p, 0);
}

static void* handle_client(void* arg) {
    int client_fd = *(int*)arg;
    free(arg);

    pthread_mutex_lock(&clients_mtx);
    active_clients++;
    pthread_mutex_unlock(&clients_mtx);

    World world;
    world_init(&world);
    send_frame(client_fd, &world, "game");

    int running = 1;

    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);

        struct timeval tv;
        tv.tv_sec = TICK / 1000000;
        tv.tv_usec = TICK % 1000000;

        int sel = select(client_fd + 1, &readfds, NULL, NULL, &tv);
        if (sel < 0) break;

        if (sel > 0 && FD_ISSET(client_fd, &readfds)) {
            unsigned char buf[256];
            ssize_t r = recv(client_fd, buf, sizeof(buf), 0);
            if (r <= 0) break;

            for (ssize_t i = 0; i < r; ++i) {
                unsigned char cmd = buf[i];

                if (cmd == 'q') { running = 0; break; }
                else if (cmd == 'r') { world_init(&world); send_frame(client_fd, &world, "Restarting..."); }
                else if (cmd == 'U' || cmd == 'D' || cmd == 'L' || cmd == 'R') world_set_direction(&world, (char)cmd);
            }
        }

        world_tick(&world);

        if (!world_is_game_over(&world)) send_frame(client_fd, &world, NULL);
        else {
            char over[128];
            sprintf(over, "GAME_OVER: score=%d", world.score);
            send_frame(client_fd, &world, over);
        }
    }

    close(client_fd);

    pthread_mutex_lock(&clients_mtx);
    active_clients--;
    if (active_clients == 0) {
        shutdown_requested = 1;
        pthread_cond_broadcast(&clients_cond);
    }
    pthread_mutex_unlock(&clients_mtx);

    return NULL;
}

int main() {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(listen_fd); return 1;
    }

    if (listen(listen_fd, 64) < 0) {
        perror("listen"); close(listen_fd); return 1;
    }

    printf("Snake server on %d (multi-client)\n", PORT);

    pthread_mutex_init(&clients_mtx, NULL);
    pthread_cond_init(&clients_cond, NULL);

    while (1) {
        pthread_mutex_lock(&clients_mtx);
        int stop = shutdown_requested;
        pthread_mutex_unlock(&clients_mtx);
        if (stop) break;

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(listen_fd, &fds);
        struct timeval tv = {0, 200000}; // 0.2s timeout

        int ready = select(listen_fd + 1, &fds, NULL, NULL, &tv);
        if (ready <= 0) continue;

        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0) continue;

        int *fd_ptr = malloc(sizeof(int));
        *fd_ptr = client_fd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(fd_ptr);
        } else {
            pthread_detach(tid);
        }
    }

    pthread_mutex_lock(&clients_mtx);
    while (active_clients > 0)
        pthread_cond_wait(&clients_cond, &clients_mtx);
    pthread_mutex_unlock(&clients_mtx);

    close(listen_fd);
    pthread_mutex_destroy(&clients_mtx);
    pthread_cond_destroy(&clients_cond);

    printf("Server shutdown cleanly.\n");
    return 0;
}
