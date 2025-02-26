#include "shudu.h"
#include "vcon.h"
#include <stdlib.h>

#define SWAP(a, b)                                                                                                     \
    do {                                                                                                               \
        typeof(a) _t_ = a;                                                                                             \
        a = b;                                                                                                         \
        b = _t_;                                                                                                       \
    } while (0)

#define COLCOL_Fix 94
#define COLCOL_Focus 91

typedef union {
    struct {
        unsigned int val : 4;
        unsigned int order : 8;
        unsigned int meybe : 16;
        unsigned int fixed : 1;
    };
    unsigned int prop;
} cell_prop;

cell_prop cell[81];
int answer[81];

int temp[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
char *data_item[] = {" ", "１", "２", "３", "４", "５", "６", "７", "８", "９"};
char *maybe_item[] = {" ", "①", "②", "③", "④", "⑤", "⑥", "⑦", "⑧", "⑨"};

int output_Y = 30;
int output_X = 80;
int curRow = 0;
int curCol = 0;

void game_start();
void draw_data();
void draw_cell(int r, int c);

int get_index(int n) { return ((n + 1) % 3 == 0) ? ((n == 8) ? 4 : 3) : 2; }

void draw_interface() {
    static char *GridItem[5][5] = {{"╔", "═", "╤", "╦", "╗"},
                                   {"║", " ", "│", "║", "║"},
                                   {"╟", "─", "┼", "╫", "╢"},
                                   {"╠", "═", "╪", "╬", "╣"},
                                   {"╚", "═", "╧", "╩", "╝"}};
    int i, r = 0, w = 7;
    vcon_reset_text(); // reset text color
    vcon_clear_text(); // clear
    vcon_move_cursor(1, 1);
    printf(GridItem[r][0]);
    for (int x = 0; x < 9; x++) { // line top
        for (i = 0; i < w; i++) {
            printf(GridItem[r][1]);
        }
        printf(GridItem[r][get_index(x)]);
    }
    for (int y = 0; y < 9; y++) {
        for (int h = 0; h < 4; h++) {
            r = (h == 3) ? get_index(y) : 1;
            vcon_move_cursor(1, 4 * y + h + 2);
            printf(GridItem[r][0]);
            for (int x = 0; x < 9; x++) {
                for (i = 0; i < w; i++) {
                    printf(GridItem[r][1]);
                }
                printf(GridItem[r][get_index(x)]);
            }
        }
    }

    vcon_set_text(7);

    vcon_move_cursor(output_X, 2);
    printf("退出(Esc)");
    vcon_move_cursor(output_X, 4);
    printf(" 新局(X) ");
    vcon_move_cursor(output_X, 6);
    printf(" 提示(T) ");

    vcon_reset_text();
}

void random_array(int *a, int count) {
    int t;
    for (int i = 0; i < count; i++) {
        t = rand() % count;
        SWAP(a[i], a[t]);
    }
}

int get_answer() {
    int cnt;
    for (int i = 0; i < 81; i++) {
        answer[i] = cell[i].val;
    }
    cnt = check_question(answer, 1);
    return cnt;
}

void make_standEnd() {
    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++) {
            if (y == 0)
                cell[x].val = x + 1;
            else if (y % 3 == 0)
                cell[y * 9 + x].val = cell[(y - 1) * 9 + (x + 4) % 9].val;
            else
                cell[y * 9 + x].val = cell[(y - 1) * 9 + (x + 3) % 9].val;
        }
}

void make_randomEnd() {
    for (int i = 0; i < 81; i++)
        cell[i].prop = 0;

    for (int g = 0; g < 3; g++) { // 随机生成\对角线三宫
        random_array(temp, 9);
        for (int y = 0; y < 3; y++)
            for (int x = 0; x < 3; x++) {
                cell[(g * 3 + y) * 9 + g * 3 + x].val = temp[y * 3 + x];
            }
    }
    get_answer(); // 生成终盘

    for (int g = 0; g < 3; g++) { // 列交换(在同一宫内)
        for (int i = 0; i < 3; i++) {
            int y1 = rand() % 3;
            int y2 = rand() % 3;
            if (y1 == y2)
                continue;
            for (int x = 0; x < 9; x++)
                SWAP(answer[(g * 3 + y1) * 9 + x], answer[(g * 3 + y2) * 9 + x]);
        }
    }
    for (int g = 0; g < 3; g++) { // 行交换(在同一宫内)
        for (int i = 0; i < 3; i++) {
            int x1 = rand() % 3;
            int x2 = rand() % 3;
            if (x1 == x2)
                continue;
            for (int y = 0; y < 9; y++)
                SWAP(answer[y * 9 + g * 3 + x1], answer[y * 9 + g * 3 + x2]);
        }
    }
    for (int i = 0; i < 3; i++) { // 宫交换
        int g1 = rand() % 3;
        int g2 = rand() % 3;
        if (g1 == g2)
            continue;

        for (int y = 0; y < 9; y++) // 垂直
            for (int x = 0; x < 3; x++) {
                SWAP(answer[y * 9 + g1 * 3 + x], answer[y * 9 + g2 * 3 + x]);
            }

        for (int x = 0; x < 9; x++) // 水平
            for (int y = 0; y < 3; y++) {
                SWAP(answer[(g1 * 3 + y) * 9 + x], answer[(g2 * 3 + y) * 9 + x]);
            }
    }

    random_array(temp, 9); // 数字随机替换
    for (int i = 0; i < 81; i++)
        answer[i] = temp[answer[i] - 1];

    { // 矩阵变换 翻转 垂直 水平 \对角线 /对角线 // 旋转 90 180 270
        if (rand() % 2)
            for (int y = 0; y < 9; y++) // 垂直
                for (int x = 0; x < 4; x++)
                    SWAP(answer[y * 9 + x], answer[y * 9 + (8 - x)]);
        if (rand() % 2)
            for (int x = 0; x < 9; x++) // 水平
                for (int y = 0; y < 4; y++)
                    SWAP(answer[y * 9 + x], answer[(8 - y) * 9 + x]);
        if (rand() % 2)
            for (int y = 0; y < 9; y++) // \对角线
                for (int x = y + 1; x < 9; x++)
                    SWAP(answer[y * 9 + x], answer[x * 9 + y]);
        if (rand() % 2)
            for (int y = 0; y < 9; y++) // \对角线
                for (int x = 0; x < (8 - y); x++)
                    SWAP(answer[y * 9 + x], answer[(8 - x) * 9 + 8 - y]);

        int buff[81];
        for (int r = rand() % 4; r > 0; r--) { // 旋转 90 180 270
            for (int y = 0; y < 9; y++)        // 旋转 (90 顺时针
                for (int x = 0; x < 9; x++)
                    buff[x * 9 + 8 - y] = answer[y * 9 + x];
            for (int i = 0; i < 81; i++)
                answer[i] = buff[i];
        }
    }

    for (int i = 0; i < 81; i++) {
        cell[i].val = answer[i];
    }
}

int dig() {
    int dig_idx[81];
    for (int i = 0; i < 81; i++)
        dig_idx[i] = i;
    random_array(dig_idx, 81);

    int total = 0, cnt = 0;
    int t, k;
    while (1) {
        for (int i = 0; i < 81; i++) {
            k = dig_idx[i];
            if (cell[k].val == 0)
                continue;
            t = cell[k].val;
            cell[k].val = 0;
            if (get_answer() == 1) {
                cnt++;
            } else {
                cell[k].val = t;
            }
        }
        if (cnt) {
            total = cnt;
            cnt = 0;
        } else {
            for (int i = 0; i < 81; i++) {
                cell[i].order = 0;
                cell[i].fixed = (cell[i].val) ? 1 : 0;
            }
            break;
        }
    }

    return total;
}

void calc_maybe() {
    for (int i = 0; i < 81; i++) {
        if (cell[i].val)
            cell[i].meybe = 0;
        else
            cell[i].meybe = 0x1ff;
    }

    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++) {
            if (cell[y * 9 + x].val) {
                int gy = y / 3 * 3;
                int gx = x / 3 * 3;
                short mask = 1 << (cell[y * 9 + x].val - 1);
                for (int i = 0; i < 9; i++) {
                    cell[y * 9 + i].meybe &= ~mask;                       // 行去除
                    cell[i * 9 + x].meybe &= ~mask;                       // 列去除
                    cell[(gy + i / 3) * 9 + (gx + i % 3)].meybe &= ~mask; // 宫去除
                }
            }
        }
}

void draw_answer() {
    int r, c;
    int y = 10;

    vcon_move_cursor(output_X, output_Y);
    printf("          ");

    for (int i = 0; i < 81; i++) {
        c = i % 9;
        r = i / 9;
        if (c == 0) {
            if (r == 3 || r == 6) {
                vcon_move_cursor(output_X, y + r + r / 3 - 1);
                printf("---+---+---");
            }
            vcon_move_cursor(output_X, y + r + r / 3);
        }

        if (c == 3 || c == 6)
            printf("|");
        if (answer[i]) {
            if (cell[i].fixed)
                vcon_set_text(94);
            printf("%d", answer[i]);
        } else
            printf(".");
        vcon_reset_text();
    }
}

int generate_data() {
    int cnt = 0;
    make_randomEnd();
    cnt = dig();
    calc_maybe();
    return cnt;
}

void move_focus(int r, int c) {
    int prvRow = curRow;
    int prvCol = curCol;
    if (r < 0)
        curRow = 8;
    else
        curRow = r % 9;

    if (c < 0)
        curCol = 8;
    else
        curCol = c % 9;

    draw_cell(prvRow, prvCol);
    draw_cell(curRow, curCol);
}

void set_number(int v) {
    int i = curRow * 9 + curCol;
    if (cell[i].fixed)
        return;
    if (v >= 0 && v <= 9) {
        cell[i].val = v;
        calc_maybe();
        draw_data();
    }

    int solved = 1;
    for (i = 0; i < 81; i++)
        if (cell[i].val != answer[i]) {
            solved = 0;
            break;
        }

    vcon_move_cursor(output_X, output_Y);
    printf((solved) ? "恭喜完成！" : "          ");
}

void onKey(int key) {
    if (key == 'x')
        game_start();
    else if (key == 'w' || key == VK_UP)
        move_focus(curRow - 1, curCol);
    else if (key == 's' || key == VK_DN)
        move_focus(curRow + 1, curCol);
    else if (key == 'a' || key == VK_LT)
        move_focus(curRow, curCol - 1);
    else if (key == 'd' || key == VK_RT)
        move_focus(curRow, curCol + 1);
    else if (key == 't')
        draw_answer();
    else if (key >= '0' && key <= '9')
        set_number(key - '0');
}

void draw_cell(int row, int col) {
    int x = 2 + col * 8;
    int y = 2 + row * 4;
    int mask = 1;
    cell_prop c = cell[row * 9 + col];

    if (col == curCol && row == curRow)
        vcon_set_text(7);

    for (int h = 0; h < 3; h++) {
        vcon_move_cursor(x, y + h);
        printf(" ");
        for (int b = 0; b < 3; b++) {
            if (c.meybe & mask)
                printf("%s ", maybe_item[h * 3 + b + 1]);
            else
                printf("  ", maybe_item[0]);
            mask = mask << 1;
        }
    }

    if (c.val) {
        vcon_move_cursor(x + 3, y + 1);
        if (c.fixed)
            vcon_set_text(94);
        else if (c.val != answer[row * 9 + col])
            vcon_set_text(91);
        printf("%s", data_item[c.val]);
    }
    vcon_reset_text();
}

void draw_data() {
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            draw_cell(r, c);
}

void draw_all() {
    draw_interface();
    draw_data();
}

void game_start() {
    generate_data();
    draw_all();
    move_focus(0, 0);
}