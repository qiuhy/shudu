#include "dlx.h"
#include <stdio.h>
#include <stdlib.h>

// 十字交叉双向循环链表 Dance Link
typedef struct {
    int Row, Col;
    int Lt, Rt, Up, Dn;
} DLX_NODE;

DLX_NODE *node; // DLX中的所有元素，包括 H + C(81*4) + R(81*9*4) = 3241
int *lcnt;      // 每列元素个数
int *row_head;  // 每行第一个元素
int *ans;
int last_node;
int ans_cnt;

dlx_answer fp_answer = NULL;

void dlx_init(int maxcol, int maxrow, int max_rowitem, dlx_answer fp) {
    fp_answer = fp;
    node = (DLX_NODE *)calloc(maxcol + maxrow * max_rowitem + 1, sizeof(DLX_NODE));
    lcnt = (int *)calloc(maxcol + 1, sizeof(int));
    row_head = (int *)calloc(maxrow, sizeof(int));
    ans = (int *)calloc(maxrow, sizeof(int));

    for (int i = 0; i <= maxcol; i++) {
        node[i].Lt = i - 1;
        node[i].Rt = i + 1;
        node[i].Up = i;
        node[i].Dn = i;
        lcnt[i] = 0;
    }

    node[0].Lt = maxcol;
    node[maxcol].Rt = 0;
    last_node = maxcol;
}

void dlx_add_node(int r, int c) {
    // 修改该结点所在列链
    last_node++;
    node[last_node].Row = r;
    node[last_node].Col = c;
    node[last_node].Up = node[c].Up;
    node[last_node].Dn = c;
    node[node[last_node].Up].Dn = last_node;
    node[c].Up = last_node;

    // 修改该结点所在行链
    if (row_head[r] == 0) {
        // 该结点所在行为空
        node[last_node].Lt = last_node;
        node[last_node].Rt = last_node;
        row_head[r] = last_node;
    } else {
        node[last_node].Lt = node[row_head[r]].Lt;
        node[last_node].Rt = row_head[r];
        node[node[last_node].Lt].Rt = last_node;
        node[node[last_node].Rt].Lt = last_node;
    }
    lcnt[c]++; // 对应列元素个数++
}

void dlx_remove_col(int c) { // 删除列元素所在列链及其中元素所对应行
    for (int i = node[c].Dn; i != c; i = node[i].Dn)
        // 枚举列链中元素
        for (int j = node[i].Rt; j != i; j = node[j].Rt) {
            // 枚举列链中元素所对应行链中元素并删除
            node[node[j].Dn].Up = node[j].Up;
            node[node[j].Up].Dn = node[j].Dn;
            lcnt[node[j].Col]--;
        }
    // 删除列链（仅需删除列元素即可删除整个列链）
    node[node[c].Lt].Rt = node[c].Rt;
    node[node[c].Rt].Lt = node[c].Lt;
}

void dlx_resume_col(int c) { // 恢复列元素所在列链及其中元素所对应行
    // 恢复列链（仅需恢复列元素即可恢复整个列链）
    node[node[c].Lt].Rt = c;
    node[node[c].Rt].Lt = c;
    for (int i = node[c].Dn; i != c; i = node[i].Dn)
        // 枚举列链中元素
        for (int j = node[i].Rt; j != i; j = node[j].Rt) {
            // 枚举列链中元素所对应行链中元素并恢复
            node[node[j].Dn].Up = j;
            node[node[j].Up].Dn = j;
            lcnt[node[j].Col]++;
        }
}

void dlx_close() {
    free(node);
    free(lcnt);
    free(row_head);
    free(ans);
}

int dlx_dance(int dep) {
    // dep表示答案的个数（搜索的层数）
    int i, j, ret = 0;
    int col = node[0].Rt;
    if (col == 0) {
        // 如果head.right==head，说明有解，输出答案
        fp_answer(ans, dep);
        return 1;
    }
    for (i = node[0].Rt; i; i = node[i].Rt) {
        // 提速（寻找元素最少的列）
        if (lcnt[i] < lcnt[col])
            col = i;
    }
    dlx_remove_col(col);
    for (i = node[col].Dn; i != col; i = node[i].Dn) {
        ans[dep] = node[i].Row;
        for (j = node[i].Rt; j != i; j = node[j].Rt) // 删除选择行链元素所在列链
            dlx_remove_col(node[j].Col);
        ret += dlx_dance(dep + 1);
        if (ret > 1)
            return ret;
        for (j = node[i].Rt; j != i; j = node[j].Rt) // 恢复选择行链元素所在列链
            dlx_resume_col(node[j].Col);
    }
    dlx_resume_col(col); // 恢复列链
    if (dep == 0)
        dlx_close();
    return ret;
}
