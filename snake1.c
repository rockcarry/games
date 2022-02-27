#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif

static void snake_feed(char *board, int bw, int bh, int ssize) {
    int rf = rand() % ((bw - 2) * (bh - 2) - ssize), i;
    for (i=0; i<bw*bh; i++) {
        if (board[i] == ' ' && !rf--) {
            board[i] = 'f'; break;
        }
    }
}

static void snake_init(char *board, int bw, int bh, int *snake, int *shead, int *stail, int *ssize) {
    int i; memset(board, ' ', bw * bh);
    for (i=0; i<bw-2; i++) board[i+1] = board[bw*bh - i - 2] = '-';
    for (i=0; i<bh-2; i++) board[bw+i*bw] = board[bw-1+bw+i*bw] = '|';
    board[0] = board[bw-1] = board[bw*(bh-1)] = board[bw*bh-1] = '+';
    board[bw/2 + bh/2*bw] = '@';
    *ssize = *shead = *stail = 1;
    snake[1] = bw * bh / 2;
    snake_feed(board, bw, bh, *ssize);
}

static void snake_draw(char *board, int bw, int bh) {
    int i, j;
#ifdef WIN32
    COORD coord = {};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#endif
    printf("i-UP, k-DOWN, j-LEFT, l-RIGHT, q-EXIT\n\n");
    for (i=0; i<bh; i++) {
        for (j=0; j<bw; j++) printf("%c ", board[j+i*bw]);
        printf("\n");
    }
}

static int snake_move(char *board, int bw, int bh, int *snake, int *shead, int *stail, int *ssize, int d) {
    int hx = snake[*shead] % bw, tx = snake[*stail] % bw, food;
    int hy = snake[*shead] / bw, ty = snake[*stail] / bw;

    switch (d) {
    case 'J': case 'j': hx--; break; // left
    case 'L': case 'l': hx++; break; // right
    case 'I': case 'i': hy--; break; // up
    case 'K': case 'k': hy++; break; // down
    default : return -1;
    }

    if (board[hx + hy * bw] == '-' || board[hx + hy * bw] == '|' || (board[hx + hy * bw] == '#' && (hx != tx || hy != ty))) {
        return 'D'; // 'D' means snake die
    }

    food = (board[hx + hy * bw] == 'f');
    board[snake[*shead]] = '#';
    (*shead)++; *shead  %= bw * bh;
    snake[*shead] = hy * bw + hx;
    board[snake[*shead]] = '@';
    if (food) {
        if (++(*ssize) == (bw - 2) * (bh - 2)) return 'W'; // 'W' means win
        snake_feed(board, bw, bh, *ssize);
        return 'E'; // means snake eat
    } else {
        if (hx != tx || hy != ty) board[snake[*stail]] = ' ';
        (*stail)++; *stail %= bw * bh;
        return 'M'; // means snake move
    }
}

int main(void) {
    int  width = 17, height = 11, snake[width * height], shead = 0, stail = 0, ssize = 0, op = 0, reinit = 1, ret;
    char board[width * height];
    do {
        if (reinit) {
            snake_init(board, width, height, snake, &shead, &stail, &ssize);
            reinit = 0; op = 0;
        }
        ret = snake_move(board, width, height, snake, &shead, &stail, &ssize, op);
        snake_draw(board, width, height);
        switch (ret) {
        case 'D': reinit = 1; printf("\ngame over !\n\npress any key to try agin ...\n"); break;
        case 'W': reinit = 1; printf("\nyou win !\n\nYou are a clever and great snake !\n\npress any key to try agin ...\n"); break;
        }
    } while ((op = getch()) && op != 'q' && op != 'Q');
    return 0;
}
