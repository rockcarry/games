#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#define GAME_WIDTH      9
#define GAME_HEIGHT     7
#define ALIVE_TABSIZE  (GAME_WIDTH * GAME_HEIGHT * 8)
#define HELP_TABSIZE   (GAME_WIDTH * GAME_HEIGHT * 16 * 1024)
#define NODE_EAT_FOOD  (1 << 0)
#define NODE_IS_ALIVE  (1 << 1)
static const int ALL_KEYS[4] = { 'E', 'D', 'S', 'F' };

typedef struct NODE {
    struct NODE *prev, *next;
    int16_t hx, hy, key;
    int16_t map [GAME_WIDTH * GAME_HEIGHT];
    uint8_t walk[GAME_WIDTH * GAME_HEIGHT];
} NODE;

static int node_extend(NODE *nodecur, NODE *nodenew, int key, int walk)
{   int ret = 0, t = 0, i, tx, ty;
    nodenew->prev = nodecur, nodenew->next = NULL, nodenew->key = key;
    if      (key == 'E') nodenew->hx = nodecur->hx, nodenew->hy = nodecur->hy - 1;
    else if (key == 'D') nodenew->hx = nodecur->hx, nodenew->hy = nodecur->hy + 1;
    else if (key == 'S') nodenew->hy = nodecur->hy, nodenew->hx = nodecur->hx - 1;
    else if (key == 'F') nodenew->hy = nodecur->hy, nodenew->hx = nodecur->hx + 1;
    if (nodenew->hx < 0 || nodenew->hx >= GAME_WIDTH || nodenew->hy < 0 || nodenew->hy >= GAME_HEIGHT) return -1; // hit wall
    if (nodecur->walk[nodenew->hx + nodenew->hy * GAME_WIDTH] > walk) return -3; // walked
    if (nodecur->map [nodenew->hx + nodenew->hy * GAME_WIDTH] > 1   ) return -2; // hit body
    if (nodecur->map [nodenew->hx + nodenew->hy * GAME_WIDTH]==-1   ) ret = NODE_EAT_FOOD; // eat food
    if (nodecur->map [nodenew->hx + nodenew->hy * GAME_WIDTH]== 1   ) ret = NODE_IS_ALIVE; // alive
    memcpy(nodenew->map , nodecur->map , sizeof(nodenew->map ));
    memcpy(nodenew->walk, nodecur->walk, sizeof(nodenew->walk));
    nodenew->map [nodenew->hx + nodenew->hy * GAME_WIDTH] = nodenew->map [nodecur->hx + nodecur->hy * GAME_WIDTH] + 1;
    nodenew->walk[nodenew->hx + nodenew->hy * GAME_WIDTH] = nodenew->walk[nodenew->hx + nodenew->hy * GAME_WIDTH] + 1;
    for (i = 0; i < GAME_WIDTH * GAME_HEIGHT; i++) {
        nodenew->map[i] -= (ret != NODE_EAT_FOOD && nodenew->map[i] > 0);
        if (nodenew->map[i] == 1) t = i;
    }
    if (!(ret & NODE_IS_ALIVE)) {
        tx = t % GAME_WIDTH, ty = t / GAME_WIDTH;
        if (  abs(tx - nodenew->hx) == 1 && abs(ty - nodenew->hy) == 1
           && (  nodenew->map[tx + nodenew->hy * GAME_WIDTH] == 0
              || nodenew->map[nodenew->hx + ty * GAME_WIDTH] == 0 ) )
        {
            ret |= NODE_IS_ALIVE;
        }
    }
    return ret;
}

static int node_alive(NODE *n)
{
    int open_head = 0, open_tail = 0, open_size = 0, ret = 0, i;
    NODE *open_table, *node;

    if (n->map[n->hx + n->hy * GAME_WIDTH] <= 4 || n->map[n->hx + n->hy * GAME_WIDTH] == GAME_WIDTH * GAME_HEIGHT) return 1;
    if (!(open_table = malloc(sizeof(NODE) * ALIVE_TABSIZE))) return 0;
    node = &(open_table[open_tail]);
    open_size++, open_tail++;
    node->hx = n->hx, node->hy = n->hy;
    memcpy(node->map , n->map, sizeof(n->map    ));
    memset(node->walk, 0     , sizeof(node->walk));

    while (open_size > 0 && open_size < ALIVE_TABSIZE) {
        node = &(open_table[open_head]);
        open_size--, open_head++; open_head %= ALIVE_TABSIZE;
        for (i = 0; i < 4 && open_size < ALIVE_TABSIZE; i++) {
            if ((ret = node_extend(node, &(open_table[open_tail]), ALL_KEYS[i], 0)) < 0) continue;
            if ((ret & NODE_IS_ALIVE)) goto done;
            open_size++, open_tail++; open_tail %= ALIVE_TABSIZE;
        }
    }

done:
    free(open_table);
    return ret > 0 && (ret & NODE_IS_ALIVE);
}

typedef struct {
    char key_list[GAME_WIDTH * GAME_HEIGHT];
    int  key_num;
    int  key_cur;
} HELPER;

static int help(int16_t *map, int16_t hx, int16_t hy, HELPER *helper)
{
    int open_head = 0, open_tail = 0, open_size = 0, ret = 0, i;
    NODE *open_table = NULL, *node = NULL;
    char* key_list = helper->key_list;
    int   key_size = sizeof(helper->key_list);

    if (helper->key_cur < helper->key_num) return helper->key_list[helper->key_cur++];
    if (!(open_table = malloc(sizeof(NODE) * HELP_TABSIZE))) goto done;
    node = &(open_table[open_tail]);
    open_size++, open_tail++;
    node->hx = hx, node->hy = hy, node->key = ' ';
    node->prev = node->next = NULL;
    memcpy(node->map , map, sizeof(node->map ));
    memset(node->walk, 0  , sizeof(node->walk));

    while (open_size > 0 && open_size < HELP_TABSIZE) {
        node = &(open_table[open_head]);
        open_size--, open_head++; open_head %= HELP_TABSIZE;
        for (i = 0; i < 4 && open_size < HELP_TABSIZE; i++) {
            if ((ret = node_extend(node, &(open_table[open_tail]), ALL_KEYS[i], 1)) < 0) continue;
            if (node_alive(&(open_table[open_tail]))) {
                if (ret & NODE_EAT_FOOD) goto done;
                open_size++, open_tail++; open_tail %= HELP_TABSIZE;
            }
        }
    }

done:
    if (ret > 0 && (ret & NODE_EAT_FOOD) && key_list) {
        for (node = &(open_table[open_tail]); node != NULL; node = node->prev) if (node->prev) node->prev->next = node;
        for (i = 0, node = open_table->next; i < key_size && node; i++, node = node->next) key_list[i] = (char)node->key;
        helper->key_cur = 1, helper->key_num = i;
        ret = helper->key_list[0];
    } else {
        printf("i can't help you ! ret: %d, head: %d, tail: %d, size: %d\n", ret, open_head, open_tail, open_size);
        helper->key_cur = 0, helper->key_num = 0;
        ret = ' ';
    }
    free(open_table);
    return ret;
}

int main(void) {
    int16_t map[GAME_WIDTH * GAME_HEIGHT] = {0}, head = GAME_WIDTH * GAME_HEIGHT / 2 - 1, newh = head + 1, die = 0, eat, key, i, n; // define variables
    HELPER  helper = {0};
    for (map[newh] = -1; !die && system("cls") == 0; ) {
        eat = map[newh] == -1, map[newh] = map[head] + 1, head = newh; // eat food and check win
        for (i = 0, n = rand() % (GAME_WIDTH * GAME_HEIGHT + 1 - map[head]); eat && i < GAME_WIDTH * GAME_HEIGHT; i++) { if (!map[i] && --n <= 0) { map[i] = -1; break; } } // new food
        for (i = 0; i < GAME_WIDTH * GAME_HEIGHT; i++) { // draw game
            map[i] -= !eat && map[i] > 0;
            printf("%c%c", i == head ? '@' : map[i] == -1 ? '$' : map[i] == 1 ? '*' : map[i] > 1 ? '#' : '.', i % GAME_WIDTH == GAME_WIDTH - 1 ? '\n' : ' ');
        }
        if (map[head] == GAME_WIDTH * GAME_HEIGHT) break; // check win
        if ((key = _getch()) == 'H' || key == 'h') key  = help(map, head % GAME_WIDTH, head / GAME_WIDTH, &helper);
        if ( key             == 'E' || key == 'e') newh = head - GAME_WIDTH; // handle key
        if ( key             == 'D' || key == 'd') newh = head + GAME_WIDTH;
        if ( key             == 'S' || key == 's') newh = head - 1;
        if ( key             == 'F' || key == 'f') newh = head + 1;
        die = newh < 0 || newh >= GAME_WIDTH * GAME_HEIGHT || (abs(head - newh) == 1 && head / GAME_WIDTH != newh / GAME_WIDTH) || (map[newh] && map[newh] != 1 && map[newh] != -1); // check die
    }
    printf("\nyou %s !\n", map[head] == GAME_WIDTH * GAME_HEIGHT ? "win" : "lost"); (void)_getch(); return 0;
}
