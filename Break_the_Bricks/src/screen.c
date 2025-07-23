//
// Created by 16905 on 25-7-20.
//
#include "ncurses.h"
#include "paddle.h"
#include "screen.h"

#include <stdlib.h>
#include <string.h>

#include "bricks.h"
#include "ball.h"

#include "game.h"

WINDOW* state_win;

/**
 * 初始化游戏和状态窗口
 * 原理：将stdscr缩小作为游戏窗口，在空出处创建新窗口作为state_win，采用box()绘制边框
 */
void setup_window() {
    // window row is 30,col is 120
    // 建立两个窗口，主界面，状态
    // 主界面大小调整
    wresize(stdscr, LINES - 5, COLS);
    // 新建积分的窗口
    state_win = newwin(5, COLS, LINES - 5, 0); // lines,cols,begin_x,begin_y

    box(stdscr, 0, 0);
    box(state_win, 0, 0);

    // 显示状态信息
    mvwprintw(state_win, 3, 2, "Controls: A/D to move, ESC to quit");

    // 刷新窗口
    wrefresh(stdscr);
    wrefresh(state_win);
}

/**
 * 更新状态窗口的积分计算
 */
void update_state_window() {
    // 清除之前的状态信息
    mvwprintw(state_win, 1, 2, "                                        ");
    mvwprintw(state_win, 2, 2, "                                        ");

    // 重新打印状态信息
    if (game_mode == BATTLE_MODE || game_mode == SINGLE_MODE) {
        mvwprintw(state_win, 1, 2, "Player Health: %d | Score: %d",
          paddle1->health, paddle1->score);
    }
    if (game_mode == BATTLE_MODE) {
        mvwprintw(state_win, 2, 2, "Computer Health: %d | Score: %d",
                  paddle2->health, paddle2->score);
    }
    if (game_mode == MOVIE_MODE) {
        mvwprintw(state_win, 1, 2, "Computer_1 Health: %d | Score: %d",
            paddle1->health, paddle1->score);
        mvwprintw(state_win, 2, 2, "Computer_2 Health: %d | Score: %d",
                  paddle2->health, paddle2->score);
    }
}

/**
 * 更新游戏窗口的画面
 * 原理：调用Paddle的清除、更新与绘制方法并刷新游戏窗口
 */
void update_game_screen(int game_mode) {
    // 始终更新挡板
    clear_paddle(paddle1);
    update_paddle(paddle1);
    print_paddle(paddle1);

    clear_ball(ball1);
    update_ball(ball1, game_mode);
    print_ball(ball1);

    if (game_mode == BATTLE_MODE || game_mode == MOVIE_MODE) {
        clear_paddle(paddle2);
        update_paddle(paddle2);
        print_paddle(paddle2);

        clear_ball(ball2);
        update_ball(ball2, game_mode);
        print_ball(ball2);
    }

    // 更新砖块
    clear_bricks();
    print_bricks();

    // 更新状态窗口
    update_state_window();

    // 刷新屏幕
    wrefresh(stdscr);
    wrefresh(state_win);
}

/**
 * 删除state_win,恢复stdscr正常大小，展示游戏结束结算画面
 */
void display_game_over_screen() {
    // 完全清理屏幕，包括状态窗口
    if (state_win) {
        wrefresh(state_win);
        delwin(state_win);
        state_win = NULL;
    }
    clear();
    wresize(stdscr, LINES , COLS);
    refresh();

    // 获取窗口尺寸
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // ASCII艺术标题 - 完全居中
    int title_y = max_y / 6 - 3;
    int ascii_width = 56;  // ASCII艺术的实际宽度
    int title_start_x = (max_x - ascii_width) / 2;

    mvprintw(title_y,     title_start_x, "  ____    _    __  __ _____    _____     _______ ____  ");
    mvprintw(title_y + 1, title_start_x, " / ___|  / \\  |  \\/  | ____|  / _ \\ \\   / / ____|  _ \\ ");
    mvprintw(title_y + 2, title_start_x, "| |  _  / _ \\ | |\\/| |  _|   | | | \\ \\ / /|  _| | |_) |");
    mvprintw(title_y + 3, title_start_x, "| |_| |/ ___ \\| |  | | |___  | |_| |\\ V / | |___|  _ < ");
    mvprintw(title_y + 4, title_start_x, " \\____/_/   \\_\\_|  |_|_____|  \\___/  \\_/  |_____|_| \\_\\");

    // 结算信息区域 - 从标题下方开始
    int info_start_y = title_y + 8;

    if (game_mode == SINGLE_MODE) {
        // 单人模式结算 - 每行都居中
        char title_text[] = "SINGLE PLAYER GAME RESULTS";
        mvprintw(info_start_y, (max_x - strlen(title_text)) / 2, "%s", title_text);

        char health_text[50];
        sprintf(health_text, "Player Health: %d", paddle1->health);
        mvprintw(info_start_y + 3, (max_x - strlen(health_text)) / 2, "%s", health_text);

        char score_text[50];
        sprintf(score_text, "Player Score: %d", paddle1->score);
        mvprintw(info_start_y + 4, (max_x - strlen(score_text)) / 2, "%s", score_text);

        // 判断游戏结果
        if (paddle1->health <= 0) {
            char result_text[] = "Result: GAME OVER";
            mvprintw(info_start_y + 7, (max_x - strlen(result_text)) / 2, "%s", result_text);

            char reason_text[] = "You ran out of lives!";
            mvprintw(info_start_y + 8, (max_x - strlen(reason_text)) / 2, "%s", reason_text);
        } else if (game_over == -1){
            char result_text[] = "Result: QUIT";
            mvprintw(info_start_y + 7, (max_x - strlen(result_text)) / 2, "%s", result_text);

            char reason_text[] = "Thanks for playing!";
            mvprintw(info_start_y + 8, (max_x - strlen(reason_text)) / 2, "%s", reason_text);
        } else {
            char result_text[] = "Result: YOU WIN!";
            mvprintw(info_start_y + 7, (max_x - strlen(result_text)) / 2, "%s", result_text);

            char reason_text[] = "You've cleared all the bricks!";
            mvprintw(info_start_y + 8, (max_x - strlen(reason_text)) / 2, "%s", reason_text);
        }

        // 评价系统
        char rating_text[60];
        if (paddle1->score >= 50) {
            sprintf(rating_text, "Rating: EXCELLENT! ★★★★★");
        } else if (paddle1->score >= 30) {
            sprintf(rating_text, "Rating: GOOD!      ★★★★☆");
        } else if (paddle1->score >= 15) {
            sprintf(rating_text, "Rating: NOT BAD!   ★★★☆☆");
        } else {
            sprintf(rating_text, "Rating: UGHHHH..   ★☆☆☆☆");
        }
        mvprintw(info_start_y + 11, (max_x - 24) / 2, "%s", rating_text);

    } else if (game_mode == BATTLE_MODE || game_mode == MOVIE_MODE) {
        // 对战模式结算 - 每行都居中
        char title_text[] = "BATTLE MODE GAME RESULTS";
        mvprintw(info_start_y, (max_x - strlen(title_text)) / 2, "%s", title_text);

        char player_text[60];
        if (game_mode == BATTLE_MODE) sprintf(player_text, "Player Health: %d    Score: %d", paddle1->health, paddle1->score);
        else sprintf(player_text, "Computer_1 Health: %d    Score: %d", paddle1->health, paddle1->score);
        mvprintw(info_start_y + 4, (max_x - strlen(player_text)) / 2, "%s", player_text);

        char ai_text[60];
        if (game_mode == BATTLE_MODE) sprintf(ai_text, "Computer Health: %d    Score: %d", paddle2->health, paddle2->score);
        else sprintf(ai_text, "Computer_2 Health: %d    Score: %d", paddle2->health, paddle2->score);
        mvprintw(info_start_y + 6, (max_x - strlen(ai_text)) / 2, "%s", ai_text);

        // 判断胜负
        char result_text[50];

        if (paddle1->score == paddle2->score) {
            sprintf(result_text, "Result: DRAW!");
        } else if (paddle1->score < paddle2->score) {
            if (game_mode == BATTLE_MODE) sprintf(result_text, "Result: COMPUTER WINS!");
            else sprintf(result_text, "Result: COMPUTER_2 WINS!");
        } else if (paddle1->score > paddle2->score) {
            if (game_mode == BATTLE_MODE) sprintf(result_text, "Result: PLAYER WINS!");
            else sprintf(result_text, "Result: COMPUTER_1 WINS!");
        } else {
            sprintf(result_text, "Result: QUIT");
        }

        mvprintw(info_start_y + 8, (max_x - strlen(result_text)) / 2, "%s", result_text);

    }

    // 底部提示信息 - 居中显示
    char* continue_text = "Press <Esc> or <Space> to continue...";
    mvprintw(max_y - 7, (max_x - strlen(continue_text)) / 2, "%s", continue_text);

    char* thanks_text = "Thanks for playing! 🎮";
    mvprintw(max_y - 5, (max_x - strlen(thanks_text)) / 2, "%s", thanks_text);

    char* credit_text = "Created by Un1Zer|Summer of 2025 in SCU";
    mvprintw(max_y - 2,(max_x - strlen(credit_text)) / 2, "%s", credit_text);

    // 添加一些装饰性的分隔线
    char* separator = "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━";
    mvprintw(info_start_y - 1, (max_x - 40) / 2, "%s", separator);
    mvprintw(info_start_y + 10, (max_x - 40) / 2, "%s", separator);

    refresh();
}