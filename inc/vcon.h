#ifndef _VCON_H_
#define _VCON_H_
// 虚拟终端序列
#include <stdio.h>

// #define ESC "\e"
#define CSI "\e["
#define vcon_end() printf(CSI "?1049l")                   // 恢复终端
#define vcon_beg() printf(CSI "?1049h")                   // 使用备用缓冲区
#define vcon_hide_cursor() printf(CSI "?25l")             // hide cursor
#define vcon_move_cursor(x, y) printf(CSI "%d;%dH", y, x) // move cursor
#define vcon_clear_text() printf(CSI "2J");               // clear
#define vcon_reset_text() printf(CSI "0m")                // reset text color
#define vcon_set_text(x) printf(CSI "%dm", x)

#define VK_ESC '\e'

#define VK_UP 0xe048 // 方向键 上
#define VK_DN 0xe050 // 方向键 下
#define VK_LT 0xe04b // 方向键 左
#define VK_RT 0xe04d // 方向键 右

#endif //_VCON_H_
