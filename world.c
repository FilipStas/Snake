#include "world.h"
#include "snake.h"

typedef struct { int x, y; } Point;

char board[BOARD_WIDTH][BOARD_HEIGHT];

static Point food;

void add_Snake_to_board(int x, int y) {

}

void fill_board() {
    int x,y;
    for (x = 0; x < BOARD_WIDTH; x++) {
        for (y = 0; y < BOARD_HEIGHT; y++) {
            if (x==0||x==BOARD_WIDTH-1||y==0||y==BOARD_HEIGHT-1) {
                board[x][y] = '#';
            } else {
                board[x][y] = ' ';
            }
        }
    }
}
void create_food() {
    srand(time(NULL));
    food.x = rand() % (BOARD_WIDTH - 2) + 1;
    food.y = rand() % (BOARD_HEIGHT - 2) + 1;
    board[food.x][food.y] = '$';
}

char* return_board() {
    int x, y;
    // dostatočne veľký buffer: každý znak + nový riadok + '\0'
    char* board_str = malloc((BOARD_WIDTH + 1) * BOARD_HEIGHT + 1);
    if (!board_str) return NULL;

    int pos = 0;
    for (y = 0; y < BOARD_HEIGHT; y++) {         // riadok po riadku
        for (x = 0; x < BOARD_WIDTH; x++) {
            board_str[pos++] = board[x][y];
        }
        board_str[pos++] = '\n';                 // nový riadok
    }
    board_str[pos] = '\0';                       // ukončovací znak

    return board_str;
}
/*
int main(int argc, char *argv[]) {
    fill_board();
    print_board();
    return 0;
}
*/