#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

static void snack_feed(char *board, int bw, int bh, int ssize) {
    int rf = rand() % ((bw - 2) * (bh - 2) - ssize), i;
    for (i=0; i<bw*bh; i++) {
        if (board[i] == ' ' && !rf--) {
            board[i] = 'f'; break;
        }
    }
}

static void snack_init(char *board, int bw, int bh, int *snack, int *shead, int *stail, int *ssize) {
    int i; memset(board, ' ', bw * bh);
    for (i=0; i<bw-2; i++) board[i+1] = board[bw*bh - i - 2] = '-';
    for (i=0; i<bh-2; i++) board[bw+i*bw] = board[bw-1+bw+i*bw] = '|';
    board[0] = board[bw-1] = board[bw*(bh-1)] = board[bw*bh-1] = '+';
    board[bw/2 + bh/2*bw] = '@';
    *ssize = *shead = *stail = 1;
    snack[1] = bw * bh / 2;
    snack_feed(board, bw, bh, *ssize);
}

static void snack_draw(char *board, int bw, int bh) {
    int i, j;
    system("cls"); printf("i-UP, k-DOWN, j-LEFT, l-RIGHT, q-EXIT\n\n");
    for (i=0; i<bh; i++) {
        for (j=0; j<bw; j++) printf("%c ", board[j+i*bw]);
        printf("\n");
    }
}

static int snack_move(char *board, int bw, int bh, int *snack, int *shead, int *stail, int *ssize, int d) {
    int hx = snack[*shead] % bw, tx = snack[*stail] % bw, food;
    int hy = snack[*shead] / bw, ty = snack[*stail] / bw;

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
    board[snack[*shead]] = '#';
    (*shead)++; *shead  %= bw * bh;
    board[snack[*shead]] = '@';
    snack[*shead] = hy * bw + hx;
    if (food) {
        if (++(*ssize) == (bw - 2) * (bh - 2)) return 'W'; // 'W' means win
        snack_feed(board, bw, bh, *ssize);
        return 'E'; // means snake eat
    } else {
        board[snack[*stail]] = ' ';
        (*stail)++; *stail %= bw * bh;
        return 'M'; // means snake move
    }
}

int main(void) {
    int  width = 17, height = 11, snack[width * height], shead = 0, stail = 0, ssize = 0, op = 0, reinit = 1, ret;
    char board[width * height];
    do {
        if (reinit) {
            snack_init(board, width, height, snack, &shead, &stail, &ssize);
            reinit = 0; op = 0;
        }
        ret = snack_move(board, width, height, snack, &shead, &stail, &ssize, op);
        snack_draw(board, width, height);
        switch (ret) {
        case 'D': reinit = 1; printf("\ngame over !\n\npress any key to try agin ...\n"); break;
        case 'W': reinit = 1; printf("\nyou win !\n\nYou are a clever and great snake !\n\npress any key to try agin ...\n"); break;
        }
    } while ((op = getch()) && op != 'q' && op != 'Q');
    return 0;
}
