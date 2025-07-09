/*
 * Snake Game in C (No external dependencies)
 * Compile: gcc snake_game.c -o snake_game
 * Run: ./snake_game
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define INITIAL_SNAKE_LENGTH 5
#define TICK_USEC 200000

// Directions
enum Direction { UP, DOWN, LEFT, RIGHT };

struct TermiosState {
    struct termios orig;
};

// Snake segment
typedef struct Segment {
    int x, y;
} Segment;

// Global state
Segment snake[WIDTH * HEIGHT];
int snake_length;
enum Direction dir;
int food_x, food_y;
bool game_over;

// Terminal control
void disable_raw_mode(struct TermiosState *state) {
    tcsetattr(STDIN_FILENO, TCSANOW, &state->orig);
}

void enable_raw_mode(struct TermiosState *state) {
    tcgetattr(STDIN_FILENO, &state->orig);
    struct termios raw = state->orig;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

// Non-blocking input
int kbhit() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) > 0;
}

int getch_nonblock() {
    if (!kbhit()) return -1;
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1)
        return c;
    return -1;
}

// Initialize game
void init_game() {
    snake_length = INITIAL_SNAKE_LENGTH;
    int start_x = WIDTH / 2;
    int start_y = HEIGHT / 2;
    for (int i = 0; i < snake_length; i++) {
        snake[i].x = start_x - i;
        snake[i].y = start_y;
    }
    dir = RIGHT;
    srand(time(NULL));
    food_x = rand() % WIDTH;
    food_y = rand() % HEIGHT;
    game_over = false;
}

// Draw
void draw() {
    // Clear screen
    printf("\033[H\033[J");

    // Top border
    for (int x = 0; x < WIDTH+2; x++) printf("#");
    printf("\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("#");
        for (int x = 0; x < WIDTH; x++) {
            bool printed = false;
            // Snake
            for (int i = 0; i < snake_length; i++) {
                if (snake[i].x == x && snake[i].y == y) {
                    printf(i==0 ? "O" : "o");
                    printed = true;
                    break;
                }
            }
            if (printed) continue;
            // Food
            if (x == food_x && y == food_y) {
                printf("*");
                continue;
            }
            printf(" ");
        }
        printf("#\n");
    }
    // Bottom border
    for (int x = 0; x < WIDTH+2; x++) printf("#");
    printf("\n");
    printf("Score: %d\n", snake_length - INITIAL_SNAKE_LENGTH);
}

// Update snake position
void update() {
    // Input
    int c = getch_nonblock();
    if (c == '\033') { // arrow keys
        getch_nonblock(); // skip '['
        char dirc = getch_nonblock();
        if (dirc == 'A' && dir != DOWN) dir = UP;
        if (dirc == 'B' && dir != UP) dir = DOWN;
        if (dirc == 'C' && dir != LEFT) dir = RIGHT;
        if (dirc == 'D' && dir != RIGHT) dir = LEFT;
    } else if (c == 'q') {
        game_over = true;
    }

    // Move snake body
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    // Move head
    switch (dir) {
        case UP: snake[0].y--; break;
        case DOWN: snake[0].y++; break;
        case LEFT: snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
    }
    // Border collision
    if (snake[0].x < 0 || snake[0].x >= WIDTH || snake[0].y < 0 || snake[0].y >= HEIGHT) {
        game_over = true;
    }
    // Self collision
    for (int i = 1; i < snake_length; i++) {
        if (snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
            game_over = true;
        }
    }
    // Food collision
    if (snake[0].x == food_x && snake[0].y == food_y) {
        snake_length++;
        food_x = rand() % WIDTH;
        food_y = rand() % HEIGHT;
    }
}

int main() {
    struct TermiosState ts;
    enable_raw_mode(&ts);
    init_game();

    while (!game_over) {
        draw();
        update();
        usleep(TICK_USEC);
    }
    disable_raw_mode(&ts);
    printf("Game Over! Final Score: %d\n", snake_length - INITIAL_SNAKE_LENGTH);
    return 0;
}
