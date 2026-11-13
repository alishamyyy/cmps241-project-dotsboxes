#ifndef INPUT_H
#define INPUT_H

#include "board.h"

int get_menu_choice(void);
void get_valid_move(const Board *b, char player, int *r1, int *c1, int *r2, int *c2);

#endif
