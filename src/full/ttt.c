/*
 * Tic-Tac-Toe in C (No external dependencies)
 * Compile: gcc tic_tac_toe.c -o tic_tac_toe
 * Run: ./tic_tac_toe
 * Two players: X and O, turn-based in terminal.
 */

#include <stdio.h>
#include <stdlib.h>

char board[3][3];

void init_board() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
}

void draw_board() {
    system("clear");
    printf("\n");
    printf(" %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[2][0], board[2][1], board[2][2]);
    printf("\n");
}

int check_win(char player) {
    // Rows, columns, diagonals
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return 1;
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return 1;
    }
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return 1;
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) return 1;
    return 0;
}

int check_draw() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == ' ') return 0;
    return 1;
}

void make_move(char player) {
    int row, col;
    while (1) {
        printf("Player %c, enter row and column (1-3 1-3): ", player);
        if (scanf("%d %d", &row, &col) != 2) {
            while (getchar() != '\n');
            printf("Invalid input. Try again.\n");
            continue;
        }
        row--; col--;
        if (row < 0 || row > 2 || col < 0 || col > 2) {
            printf("Out of range. Try again.\n");
        } else if (board[row][col] != ' ') {
            printf("Cell occupied. Try again.\n");
        } else {
            board[row][col] = player;
            break;
        }
    }
}

int main() {
    init_board();
    char current = 'X';
    while (1) {
        draw_board();
        make_move(current);
        if (check_win(current)) {
            draw_board();
            printf("Player %c wins!\n", current);
            break;
        }
        if (check_draw()) {
            draw_board();
            printf("It's a draw!\n");
            break;
        }
        current = (current == 'X') ? 'O' : 'X';
    }
    return 0;
}
