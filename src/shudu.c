#include "dlx.h"
#include <stdio.h>
#include <stdlib.h>

int sd[81];

void add_num(int i, int v) {
    int c, r, x, y, g;
    r = i * 9 + v - 1;
    x = i % 9;
    y = i / 9;
    g = (y / 3) * 3 + x / 3;

    c = i + 1; // 1-81 (x,y)中有数字
    dlx_add_node(r, c);

    c = x * 9 + v + 81; // 82-162 x行中有数字v
    dlx_add_node(r, c);

    c = y * 9 + v + 162; // 163-243 y列中有数字v
    dlx_add_node(r, c);

    c = g * 9 + v + 243; // 244-324 所在宫中有数字v
    dlx_add_node(r, c);
}

void on_answer(int *ans, int count) {
    int row, v;
    for (int i = 0; i < count; i++) {
        v = ans[i] % 9 + 1;
        sd[ans[i] / 9] = v;
    }
}

int check_question(int *question, int output) {
    dlx_init(81 * 4, 81 * 9, 4, &on_answer);
    for (int i = 0; i < 81; i++) {
        sd[i] = question[i];
        if (sd[i]) {
            add_num(i, sd[i]);
        } else {
            for (int v = 1; v <= 9; v++)
                add_num(i, v);
        }
    }
    int ans = dlx_dance(0);
    if (ans > 0 && output)
        for (int i = 0; i < 81; i++)
            question[i] = sd[i];

    return ans;
}