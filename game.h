#ifndef GAME_H
#define GAME_H

#include "board.h"

int is_edge_taken(const Board *b, int r1, int c1, int r2, int c2);
int make_move(Board *b, int r1, int c1, int r2, int c2, char player);
int is_game_over(const Board *b);

#endif
