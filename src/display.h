#ifndef DISPLAY_H
#define DISPLAY_H

#include "board.h" // from malek

void show_menu(void);
void print_board(const Board *b);
void print_scores(const Board *b);
void print_winner(const Board *b);

#endif
