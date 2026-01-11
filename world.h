#ifndef WORLD_H
#define WORLD_H

#define WIDTH 20
#define HEIGHT 15
#define WORLD_MAX (WIDTH * HEIGHT)

typedef struct { int x, y; } Pos;

typedef struct {
    Pos snake[WORLD_MAX];
    int len;
    char dir;       // 'U' 'D' 'L' 'R'
    char next_dir;
    Pos fruit;
    int score;
    int game_over;  // 0 running, 1 over
} World;

void world_init(World *world);
void world_set_direction(World *world, char dir);
void world_tick(World *world);
int world_is_game_over(const World *world);

#endif
