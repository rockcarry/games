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

void draw_game(int board[20][10], char curbricks[2], int curx, int cury,
                      char nextbricks[2], int level, int score, int lines, int history, int gameover);

int check_collision(int board[20][10], char bricks[2], int curx, int cury);

void copy_bricks_to_board(int board[20][10], char bricks[2], int curx, int cury);

int mark_full_rows(int board[20][10]);

void remove_full_rows(int board[20][10]);

void execute_bricks(void);