#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "game.h"

static void clear_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        // discard remaining characters
    }
}

int get_menu_choice(void) {
    int choice;
    while (1) {
        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= 3) {
            clear_input_buffer();
            return choice;
        }
        clear_input_buffer();
        printf("Invalid choice. Please enter 1, 2, or 3: ");
    }
}

void get_valid_move(const Board *b, char player, int *r1, int *c1, int *r2, int *c2) {
    while (1) {
        printf("Player %c's turn. Enter the row and column of the first dot (e.g., A0 -> 0 0) and second dot:\n", player);

        if (scanf("%d %d %d %d", r1, c1, r2, c2) != 4) {
            clear_input_buffer();
            printf("Invalid input format. Enter 4 numbers separated by spaces (e.g., 0 0 0 1).\n");
            continue;
        }
        clear_input_buffer();

        // 1. Check bounds (rows 0..4, cols 0..5)
        if (*r1 < 0 || *r1 >= DOT_ROWS || *c1 < 0 || *c1 >= DOT_COLS ||
            *r2 < 0 || *r2 >= DOT_ROWS || *c2 < 0 || *c2 >= DOT_COLS) {
            printf("Out of bounds! Rows must be 0-%d and columns 0-%d.\n", DOT_ROWS - 1, DOT_COLS - 1);
            continue;
        }

        // 2. Check Manhattan distance == 1 (adjacent horizontal or vertical dots only)
        int dist = abs(*r1 - *r2) + abs(*c1 - *c2);
        if (dist != 1) {
            printf("Invalid move! Dots must be directly adjacent horizontally or vertically.\n");
            continue;
        }

        // 3. Check if the line is already drawn
        if (is_edge_taken(b, *r1, *c1, *r2, *c2)) {
            printf("That line is already taken! Choose another edge.\n");
            continue;
        }

        break;
    }
}
