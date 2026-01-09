//
// Created by Filip on 9. 1. 2026.
//
#include "snake.h"

typedef struct SnakeSegment {
    int x;
    int y;
    struct SnakeSegment* next;
} SnakeSegment;

SnakeSegment* head = NULL;
SnakeSegment* tail = NULL;


void create_snake(int x, int y) {
    head = (SnakeSegment*)malloc(sizeof(SnakeSegment));
    if (!head) return;
    head->x = x;
    head->y = y;
    head->next = NULL;

    tail = head;
}

void enlarge_snake(void) {
    if (!tail) return;

    SnakeSegment* new_segment = malloc(sizeof(SnakeSegment));
    if (!new_segment) return;


    new_segment->x = tail->x;
    new_segment->y = tail->y;
    new_segment->next = NULL;

    tail->next = new_segment;
    tail = new_segment;
}

void move_snake(direction_t dir) {
    if (!head) return;

    int prev_x = head->x;
    int prev_y = head->y;
    // posun hlavy
    switch (dir) {
        case UP:    head->y -= 1; break;
        case DOWN:  head->y += 1; break;
        case LEFT:  head->x -= 1; break;
        case RIGHT: head->x += 1; break;
    }

    SnakeSegment* current = head->next;
    while (current) {
        int tmp_x = current->x;
        int tmp_y = current->y;

        current->x = prev_x;
        current->y = prev_y;

        prev_x = tmp_x;
        prev_y = tmp_y;

        current = current->next;
    }
}
// ----------------------
// Uvoľnenie hada
// ----------------------
void free_snake(void) {
    SnakeSegment* current = head;
    while (current) {
        SnakeSegment* tmp = current;
        current = current->next;
        free(tmp);
    }
    head = tail = NULL;
}
char* get_snake() {

    return NULL;
}
