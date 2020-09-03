#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#ifdef WIN32
#include <windows.h>
#define usleep(t) Sleep((t)/1000)
#define kbhit _kbhit
#define getch _getch
#endif

static char g_all_bricks[7][4][4] = {
    { { 0xF0, 0x00, 4, 1 }, { 0x88, 0x88, 1, 4 }, { 0xF0, 0x00, 4, 1 }, { 0x88, 0x88, 1, 4 } }, // I
    { { 0xE8, 0x00, 3, 2 }, { 0xC4, 0x40, 2, 3 }, { 0x2E, 0x00, 3, 2 }, { 0x88, 0xC0, 2, 3 } }, // L
    { { 0xE2, 0x00, 3, 2 }, { 0x44, 0xC0, 2, 3 }, { 0x8E, 0x00, 3, 2 }, { 0xE8, 0x80, 2, 3 } }, // L'
    { { 0xCC, 0x00, 2, 2 }, { 0xCC, 0x00, 2, 2 }, { 0xCC, 0x00, 2, 2 }, { 0xCC, 0x00, 2, 2 } }, // O
    { { 0xE4, 0x00, 3, 2 }, { 0x4C, 0x40, 2, 3 }, { 0x4E, 0x00, 3, 2 }, { 0x8C, 0x80, 2, 3 } }, // T
    { { 0xC6, 0x00, 3, 2 }, { 0x4C, 0x80, 2, 3 }, { 0xC6, 0x00, 3, 2 }, { 0x4C, 0x80, 2, 3 } }, // Z
    { { 0x6C, 0x00, 3, 2 }, { 0x8C, 0x40, 2, 3 }, { 0x6C, 0x00, 3, 2 }, { 0x8C, 0x40, 2, 3 } }, // Z'
};

static void draw_game(int board[20][10], char curbricks[2], int curx, int cury,
                      char nextbricks[2], int level, int score, int lines, int history, int gameover)
{
    int  i, j, c;
    system("cls");
    for (i=0; i<20; i++) {
        for (j=0; j<10; j++) {
            c = board[i][j] == 1 ? 'B' : board[i][j] == 2 ? 'x' : '.';
            if (j>=curx && j<curx+4 && i>=cury && i<cury+4) {
                int tx = j - curx, ty = i - cury;
                if (curbricks[ty / 2] & (1 << (3 - tx + ((ty&1) ? 0 : 4)))) {
                    c = 'B';
                }
            }
            printf("%c ", c);
        }
        switch (i) {
        case 0 : printf(" next:"); break;
        case 7 : printf(" level  : %d", level  ); break;
        case 9 : printf(" score  : %d", score  ); break;
        case 11: printf(" lines  : %d", lines  ); break;
        case 13: printf(" history: %d", history); break;
        case 15: printf("%s", gameover ? " game over !" : ""); break;
        case 16: printf("%s", gameover ? " press R to again." : ""); break;
        case 18: printf(" ffbricks v1.0.0"); break;
        case 19: printf(" writen by chenk"); break;
        }
        if (i >= 2 && i <= 5) {
            printf(" ");
            for (j=0; j<4; j++) {
                int tx = j, ty = i - 2;
                printf("%c ", nextbricks[ty / 2] & (1 << (3 - tx + ((ty&1) ? 0 : 4))) ? 'B' : ' ');
            }
        }
        printf("\n");
    }
    printf("-------------------");
}

static int check_collision(int board[20][10], char bricks[2], int curx, int cury)
{
    int i, j;
    for (i=0; i<20; i++) {
        for (j=0; j<10; j++) {
            if (j>=curx && j<curx+4 && i>=cury && i<cury+4) {
                int tx = j - curx, ty = i - cury;
                if (board[i][j] && (bricks[ty / 2] & (1 << (3 - tx + ((ty&1) ? 0 : 4))))) return 1;
            }
        }
    }
    return 0;
}

static void copy_bricks_to_board(int board[20][10], char bricks[2], int curx, int cury)
{
    int i, j;
    for (i=0; i<20; i++) {
        for (j=0; j<10; j++) {
            if (j>=curx && j<curx+4 && i>=cury && i<cury+4) {
                int tx = j - curx, ty = i - cury;
                if (bricks[ty / 2] & (1 << (3 - tx + ((ty&1) ? 0 : 4)))) {
                    board[i][j] = 1;
                }
            }
        }
    }
}

static int mark_full_rows(int board[20][10])
{
    int i, j, l = 0;
    for (i=0; i<20; i++) {
        for (j=0; j<10; j++) {
            if (!board[i][j]) {
                break;
            }
        }
        if (j == 10) { l++; for (j=0; j<10; j++) board[i][j] = 2; }
    }
    return l;
}

static void remove_full_rows(int board[20][10])
{
    int  i, j, f, m;
    for (i=19,m=19; i>=0 && m>=0; m--) {
        for (f=0,j=0; j<10; j++) {
            if ( board[m][j] == 1) { f = 1; break; }
        }
        if (f) {
            for (j=0; j<10; j++) board[i][j] = board[m][j];
            i--;
        };
    }
    for (; i>=0; i--) for (j=0; j<10; j++) board[i][j] = 0;
}

int main(void)
{
    int board[20][10] = {0};
    int type, rot, next, nrot, posx, posy, newx, newy, newr, collision, speed;
    int remove = 0, score = 0, lines = 0, level = 0, history = 0, gameover = 0;
    int quit = 0, key = 0, counter = 0, dirty = 1;

    srand((unsigned int)time(NULL));
    type = rand()%7, rot = rand()%4, posx = rand()%6, posy = 0, next = rand()%7, nrot = rand()%4;

    while (!quit) {
        if (dirty) {
            draw_game(board, g_all_bricks[type][rot], posx, posy, g_all_bricks[next][nrot], level, score, lines, history, gameover);
            dirty = 0;
        }
        if (remove) { remove_full_rows(board); remove = 0; usleep(10*1000); }

        if (!kbhit()) {
            usleep(16*1000);
            if (gameover) continue;
            speed = 60 - 58 * level / 10;
            if (speed < 2) speed = 2;
            if (++counter % speed == 0) {
                key = 'd';
            } else continue;
        } else key = getch();

        if (gameover) {
            if (key == 'R' || key == 'r') {
                memset(board, 0, sizeof(board));
                score= 0, lines = 0, level = 0, gameover = 0;
            } else continue;
        }
        newx = posx; newy = posy; newr = rot; dirty = 1;
        switch (key) {
        case 'Q': case 'q': quit = 1; break;
        case 'S': case 's': if (newx > 0) newx--; break;
        case 'F': case 'f': newx++; break;
//      case 'E': case 'e': if (newy > 0) newy--; break;
        case 'D': case 'd': newy++; break;
        case 'J': case 'j': newr++; newr %= 4; break;
        case 'K': case 'k':
            while (++newy + g_all_bricks[type][newr][3] <= 20 && !check_collision(board, g_all_bricks[type][newr], newx, newy));
            posy = newy - 1;
            break;
        }

        collision = check_collision(board, g_all_bricks[type][newr], newx, newy);
        if (  newx + g_all_bricks[type][newr][2] <= 10
           && newy + g_all_bricks[type][newr][3] <= 20 && !collision) {
            posx = newx; posy = newy; rot = newr;
        } else if (key == 'D' || key == 'd' || key == 'K' || key == 'k') {
            copy_bricks_to_board(board, g_all_bricks[type][rot], posx, posy);
            remove = mark_full_rows(board);
            lines += remove; score += remove * remove; level = score / 100; history = history > score ? history : score;
            type = next, rot = nrot, posx = rand()%6, posy = 0; next = rand()%7; nrot = rand()%4;
            gameover = check_collision(board, g_all_bricks[type][rot], posx, posy);
        }
    }
    return 0;
}
