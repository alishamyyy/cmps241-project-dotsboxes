#include <stdio.h>
#include "board.h" // from malek
#include "game.h" // from malek
#include "display.h" // from ali 
#include "input.h"  // from ali

static void run_game(void) {
    Board board;
    init_board(&board);

    char current_player = 'A';
    int r1, c1, r2, c2;

    while (!is_game_over(&board)) {
        print_board(&board);
        get_valid_move(&board, current_player, &r1, &c1, &r2, &c2);

        int boxes_completed = make_move(&board, r1, c1, r2, c2, current_player);
        print_scores(&board);

        // Switch turns ONLY if no box was completed
        if (boxes_completed == 0) {
            current_player = (current_player == 'A') ? 'B' : 'A';
        }
    }

    print_board(&board);
    print_winner(&board);
}

int main(void) {
    while (1) {
        show_menu();
        int choice = get_menu_choice();

        if (choice == 1) {
            run_game();
        } else if (choice == 2) {
            printf("\nRules: Connect two adjacent dots by entering their row and column coordinates.\n");
            printf("Completing the 4th side of a box claims it with your initial and grants an extra turn!\n\n");
        } else if (choice == 3) {
            printf("Exiting Dots and Boxes. Goodbye!\n");
            break;
        }
    }
    return 0;
}
