#ifndef BOARD_H
#define BOARD_H

#define BOX_ROWS 4
#define BOX_COLS 5
#define DOT_ROWS (BOX_ROWS + 1) // 5 rows 
#define DOT_COLS (BOX_COLS + 1) // 6 cols 

typedef struct {
    int h_lines[DOT_ROWS][BOX_COLS]; 
    int v_lines[BOX_ROWS][DOT_COLS]; 
    char boxes[BOX_ROWS][BOX_COLS]; // char holder to identify the 2 players
    int score_a;
    int score_b;
} Board;

void init_board(Board *b);

#endif
