//
// Created by 16905 on 25-7-20.
//
#include "paddle.h"
#include "game.h"
#include <unistd.h>
#include "curses.h"
#include "key.h"
#include "screen.h"
#include "bricks.h"

int game_mode = 0;
int game_over = 0;

/**
 * 启动游戏
 * 原理：初始化游戏与状态窗口，初始化键盘监听器
 * 随后用while循环调用update_game_screen()和update_state_win()实时更新游戏和积分画面
 */
void start_game() {
    // 初始化窗口
    setup_window();
    // 初始化键盘监听器
    init_listener();
    // 初始化砖块
    init_bricks(game_mode);

    int frame = 1;

    // 简单减速：不同模式不同参数
    int update_interval = (game_mode == SINGLE_MODE) ? 3 : 4;  // 单人5帧，双人8帧
    int delay_time = (game_mode == SINGLE_MODE) ? 50000 : 60000;  // 增加延迟

    if (game_mode == MOVIE_MODE) paddle1->belong_to = BELONG_TO_COMPUTER;

    while (paddle1->health > 0 && paddle2->health > 0 && !game_over) {
        if (frame % update_interval == 0) {
            update_game_screen(game_mode);
            frame = 1;
        }
        frame++;
        usleep(delay_time);  // 比原来的50000更长
    }

    // 游戏结束，显示结算画面
    display_game_over_screen();
    sleep(1);

    // 等待用户按键
    nodelay(stdscr, FALSE);  // 改为阻塞模式
    char c;
    while ((c = getch()) != KEY_ESC && c != KEY_SPACE) {}
    clear();
}