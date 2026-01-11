#include "world.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

static int pos_equal(const Pos *p1, const Pos *p2) {
    if (p1->x == p2->x && p1->y == p2->y)
        return 1;
    return 0;
}

static int hit_pos(const World *world, const Pos *pos) {
    for (int i = 0; i < world->len; i++) {
        if (pos_equal(&world->snake[i], pos))
            return 1;
    }
    return 0;
}

static void place_fruit(World *world) {
    Pos p;

    for (int i = 0; i < 10000; i++) {
        p.x = rand() % WIDTH;
        p.y = rand() % HEIGHT;

        if (!hit_pos(world, &p)) {
            world->fruit = p;
            return;
        }
    }

    world->fruit.x = -1;
    world->fruit.y = -1;
}

void world_init(World *world) {
    memset(world, 0, sizeof(World));

    srand(time(NULL));

    world->len = 3;
    world->score = 0;
    world->game_over = 0;

    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;

    world->snake[0].x = cx;
    world->snake[0].y = cy;

    world->snake[1].x = cx - 1;
    world->snake[1].y = cy;

    world->snake[2].x = cx - 2;
    world->snake[2].y = cy;

    world->dir = 'R';
    world->next_dir = 'R';

    place_fruit(world);
}

void world_set_direction(World *world, char dir) {
    if ((dir == 'U' && world->dir == 'D') ||
        (dir == 'D' && world->dir == 'U') ||
        (dir == 'L' && world->dir == 'R') ||
        (dir == 'R' && world->dir == 'L')) return;
    world->next_dir = dir;
}

void world_tick(World *world) {
    if (world->game_over)
        return;

    world->dir = world->next_dir;

    Pos head = world->snake[0];

    if (world->dir == 'U') head.y--;
    else if (world->dir == 'D') head.y++;
    else if (world->dir == 'L') head.x--;
    else if (world->dir == 'R') head.x++;

    if (head.x < 0 || head.x >= WIDTH ||
        head.y < 0 || head.y >= HEIGHT) {
        world->game_over = 1;
        return;
        }

    if (hit_pos(world, &head)) {
        world->game_over = 1;
        return;
    }

    for (int i = world->len; i > 0; i--) {
        world->snake[i] = world->snake[i - 1];
    }

    world->snake[0] = head;

    if (pos_equal(&head, &world->fruit)) {
        if (world->len < WORLD_MAX)
            world->len++;

        world->score++;
        place_fruit(world);
    }
}

int world_is_game_over(const World *world) {
    return world->game_over;
}
