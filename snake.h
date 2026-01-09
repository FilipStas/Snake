//
// Created by Filip on 9. 1. 2026.
//

#ifndef SNAKE_SNAKE_H
#define SNAKE_SNAKE_H

#include <stddef.h>
#include "direction.h"
#include <stdlib.h>
void create_snake(int x, int y);
void enlarge_snake(void);
void move_snake(direction_t dir);
void free_snake(void);
char* get_snake();


#endif //SNAKE_SNAKE_H