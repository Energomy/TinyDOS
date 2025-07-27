#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define WIDTH 40
#define HEIGHT 20

int bird_y;
int bird_vel;
int pipe_x;
int pipe_gap_y;
int score;

void setup() {
    bird_y = HEIGHT / 2;
    bird_vel = 0;
    pipe_x = WIDTH;
    pipe_gap_y = rand() % (HEIGHT - 6) + 3;
    score = 0;
}

void draw() {
    system("clear");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 5 && y == bird_y) {
                printf(">");
            } else if (x == pipe_x) {
                if (y < pipe_gap_y - 2 || y > pipe_gap_y + 2) {
                    printf("|");
                } else {
                    printf(" ");
                }
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
}

void update() {
    bird_vel += 1;
    bird_y += bird_vel;

    if (bird_y < 0) bird_y = 0;
    if (bird_y > HEIGHT - 1) bird_y = HEIGHT - 1;

    pipe_x--;
    if (pipe_x < 0) {
        pipe_x = WIDTH;
        pipe_gap_y = rand() % (HEIGHT - 6) + 3;
        score++;
    }

    if (pipe_x == 5) {
        if (bird_y < pipe_gap_y - 2 || bird_y > pipe_gap_y + 2) {
            setup();
        }
    }
}

int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int main() {
    setup();
    while (1) {
        if (kbhit()) {
            getchar();
            bird_vel = -3;
        }
        update();
        draw();
        usleep(100000);
    }
    return 0;
}
