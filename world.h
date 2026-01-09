//
// Created by Filip on 9. 1. 2026.
//

#ifndef SNAKE_SNAKE_H
#define SNAKE_SNAKE_H

#define BOARD_WIDTH 20
#define BOARD_HEIGHT 20

#include <stdio.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>

void fill_board();
void create_food();
void add_Snake_to_board(int x, int y);

char* return_board();

#endif //SNAKE_SNAKE_H