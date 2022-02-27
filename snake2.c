#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
int main(void) {
    int map[19 * 15] = {0}, head = 19 * 15 / 2 - 1, newh = head + 1, die = 0, eat, key, i, n; // define variables
    for (map[newh] = -1; !die && system("cls") == 0; ) {
        if (eat = map[newh] == -1, map[newh] = map[head] + 1, head = newh, map[head] == 19 * 15) break; // eat food and check win
        for (i = 0, n = rand() % (19 * 15 - map[head]); eat && i < 19 * 15; i++) { if (!map[i] && --n == -1) { map[i] = -1; break; } } // new food
        for (i = 0; i < 19 * 15; i++) { // draw game
            map[i] -= !eat && map[i] > 0;
            printf("%c%c", i == head ? '@' : map[i] == -1 ? '$' : map[i] != 0 ? '*' : '.', i % 19 == 19 - 1 ? '\n' : ' ');
        }
        if ((key = _getch()) == 'E' || key == 'e') newh = head - 19; // handle key
        if ( key             == 'D' || key == 'd') newh = head + 19;
        if ( key             == 'S' || key == 's') newh = head - 1;
        if ( key             == 'F' || key == 'f') newh = head + 1;
        die = newh < 0 || newh >= 19 * 15 || (abs(head - newh) == 1 && head / 19 != newh / 19) || (map[newh] && map[newh] != 1 && map[newh] != -1); // check die
    }
    printf("\nyou %s !\n", map[head] == 19 * 15 ? "win" : "lost"); _getch(); return 0;
}