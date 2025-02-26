#include "game.h"
#include "vcon.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void onStart();

void processOutput() { fflush(stdout); }

int processInput() {
    int key;
    if (_kbhit()) {
        key = getch();
        if (key == 0xE0) {
            key = key << 8;
            key = key | getch();
        }
        if (key == VK_ESC)
            return 0;
        else
            onKey(key);
    }
    return 1;
}

void onStart() { game_start(); }

void init() {
    srand(time(NULL));
    system("chcp 65001 > nul"); // 修改字符集 支持UTF-8
    setvbuf(stdout, NULL, _IOFBF, 4096); // 设置全局缓冲
    vcon_beg();                          // 使用备用缓冲区
    vcon_hide_cursor();
    onStart();
}

void onQuit() {
    vcon_end();
    exit(0);
}

int main() {
    init();
    while (processInput())
        processOutput();
    onQuit();
}
