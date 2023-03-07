#include "../inc/2048.h"

#define GAME_SIZE  4

void left(int *data)
{
    int i, j, f;
    int row = GAME_SIZE;
    while (row--) {
        for (j=0,f=0,i=0; i<GAME_SIZE; i++) {
            if (data[i]) {
                if (!f && j>0 && data[j-1] == data[i]) {
                    data[j-1] += data[i]; 
                    f = 1;              // represent one merge
                } 
                else 
                {
                    data[j++] = data[i]; 
                    f = 0;              // represent no merge
                }
            }
        }
        while (j < GAME_SIZE) data[j++] = 0;
        data += GAME_SIZE;              // every iteration, the data should at the first place of a row
    }
}

void right(int *data)
{
    int i, j, f;
    int row = GAME_SIZE;
    while (row--) {
        for (j=GAME_SIZE-1,f=0,i=GAME_SIZE-1; i>=0; i--) {
            if (data[i]) {
                if (!f && j<GAME_SIZE-1 && data[j+1] == data[i]) {
                    data[j+1]+= data[i]; 
                    f = 1;              // represent one merge
                } else {
                    data[j--] = data[i]; 
                    f = 0;              // represent one merge
                }
            }
        }
        while (j >= 0) data[j--] = 0;
        data += GAME_SIZE;              // every iteration, the data should at the first place of a row
    }
}

void up(int *data)
{
    int i, j, f;
    int col = GAME_SIZE;
    while (col--) {
        for (j=0,f=0,i=0; i<GAME_SIZE; i++) {
            if (data[i*GAME_SIZE]) {
                if (!f && j>0 && data[(j-1)*GAME_SIZE] == data[i*GAME_SIZE]) {
                    data[(j-1)*GAME_SIZE]+= data[i*GAME_SIZE]; 
                    f = 1;              // represent one merge
                } else {
                    data[(j++)*GAME_SIZE] = data[i*GAME_SIZE]; 
                    f = 0;              // represent one merge
                }
            }
        }
        while (j < GAME_SIZE) data[(j++)*GAME_SIZE] = 0;
        data++;                         // every iteration, the data should at the first place of a row
    }
}

void down(int *data)
{
    int i, j, f;
    int col = GAME_SIZE;
    while (col--) {
        for (j=GAME_SIZE-1,f=0,i=GAME_SIZE-1; i>=0; i--) {
            if (data[i*GAME_SIZE]) {
                if (!f && j<GAME_SIZE-1 && data[(j+1)*GAME_SIZE] == data[i*GAME_SIZE]) {
                    data[(j+1)*GAME_SIZE]+= data[i*GAME_SIZE]; 
                    f = 1;          // represent one merge
                } else {
                    data[(j--)*GAME_SIZE] = data[i*GAME_SIZE]; 
                    f = 0;          // represent one merge
                }
            }
        }
        while (j >= 0) data[(j--)*GAME_SIZE] = 0;
        data++;                     // every iteration, the data should at the first place of a row
    }
}

int next(int *data)
{
    int empidx[GAME_SIZE*GAME_SIZE];            // indicates the empty space and its number
    int empnum = 0;
    int max    = 0;
    int i, j;

    for (j=0,i=0; i<GAME_SIZE*GAME_SIZE; i++) {
        if (data[i] == 0) {
            empidx[j++] = i;
            empnum++;
        }
        max = max > data[i] ? max : data[i];
    }
    if (empnum) {
        data[empidx[rand()%empnum]] = 1;
        return max == 2048 ? 1 : 0;
    }
    else return -1;
}

void output(int *data)
{
    int i, sum = 0;
#ifdef WIN32
    COORD coord = {};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#endif
    printf("+--------------------+\n\n");
    for (i=1; i<=GAME_SIZE*GAME_SIZE; i++) {
        if (0 || data[i-1]) {
            printf("%4d %s", data[i-1], i%GAME_SIZE ? "" : "\n\n");
        } else {
            printf("%s %s" , "   ."   , i%GAME_SIZE ? "" : "\n\n");
        }
        sum += data[i-1];
    }
    printf("+--------------------+\n\n");
    printf("������%4d\n\n", sum);
}

void execute_2048(void)
{
    int data[GAME_SIZE * GAME_SIZE] = {0};
    int ret = 0;

    // At the beginning, release two places
    next  (data);
    next  (data);
    output(data);

    while (1) {
        int c = getch();
        switch (c) {
        case 'j': left (data); break;
        case 'k': down (data); break;
        case 'l': right(data); break;
        case 'i': up   (data); break;
        case 'q': return 0;
        default : continue;
        }
        ret = next(data);
        output(data);

        if (ret == -1) printf("game over !\n");
        if (ret ==  1) printf("you  win  !\n");
    }
}

