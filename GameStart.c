#include "inc/2048.h"
#include "inc/bricks.h"

int main(void)
{
    while (1)
    {
        printf("choose the game \n");

        int c = getch();
        system("cls");
        switch (c)
        {
        case '2':
            execute_2048();
            break;
        
        case 'b':
            execute_bricks();
            break;
        
        case 'q':
            return -1;
            break;
        
        default:
            printf("please select from '2', 'b' or 'q(quit)'");
            break;
        }
    }
    
    return 1;
}