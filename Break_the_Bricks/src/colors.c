//
// Created by 16905 on 25-7-20.
//
#include "ncurses.h"
#include "colors.h"

#include <stdlib.h>

#include "bricks.h"

#define RED2 20
#define BLUE2 21
#define GREEN2 22
#define YELLOW_BRIGHT 23
#define YELLOW_LIGHT 24
#define YELLOW_DEEP 25
#define PURPLE 26
#define BLUE3 27

/**
 * 初始化颜色系统
 * 自定义颜色,并初始化砖块、球、挡板颜色对
 */
void init_colors() {
    // 如果终端不支持颜色，直接返回
    if (!has_colors()) {
        endwin();
        exit(1);
    }
    start_color();
    init_color(RED2,500,0,0);
    init_color(BLUE2,0,0,500);
    init_color(GREEN2,0,500,0);
    init_color(YELLOW_BRIGHT,400,600,0);
    init_color(YELLOW_LIGHT,250,350,0);
    init_color(YELLOW_DEEP,650,550,0);
    init_color(PURPLE,600,600,0);
    init_color(BLUE3,0,0,850);
    // 普通砖块：黄色 BOOM MAGENTA
    // 珍贵砖块：绿色 YEAH GREEN
    // 扣血砖块：红色 OOPS MAGENTA
    // 回血砖块：蓝色 HELP CYAN
    // 加速砖块：白色 DASH YELLOW_BRIGHT
    init_pair(NORMAL_BRICK_COMPLETE_COLOR, YELLOW_DEEP, COLOR_BLACK);     // 砖块生命值 3
    init_pair(NORMAL_BRICK_HURT_COLOR, COLOR_YELLOW, COLOR_BLACK);  // 生命值 2
    init_pair(NORMAL_BRICK_BROKEN_COLOR, YELLOW_LIGHT, COLOR_BLACK);   // 生命值 1

    init_pair(PRECIOUS_BRICK_COMPLETE_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(PRECIOUS_BRICK_BROKEN_COLOR, GREEN2, COLOR_BLACK);

    init_pair(HURT_BRICK_COMPLETE_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(HURT_BRICK_BROKEN_COLOR, RED2, COLOR_BLACK);

    init_pair(SPEED_BRICK_COMPLETE_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(SPEED_BRICK_BROKEN_COLOR, COLOR_WHITE, COLOR_BLACK);

    init_pair(HELP_BRICK_COMPLETE_COLOR, COLOR_BLUE, COLOR_BLACK);
    init_pair(HELP_BRICK_BROKEN_COLOR, BLUE2, COLOR_BLACK);

    init_pair(BOOM_MESSAGE_COLOR, COLOR_MAGENTA, COLOR_BLACK); // 爆炸信息
    init_pair(OOPS_MESSAGE_COLOR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(HELP_MESSAGE_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(DASH_MESSAGE_COLOR, YELLOW_BRIGHT, COLOR_BLACK);

    init_pair(PADDLE1_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(BALL1_COLOR, YELLOW_BRIGHT, COLOR_BLACK);
    init_pair(PADDLE2_COLOR, BLUE3, COLOR_BLACK);
    init_pair(BALL2_COLOR, PURPLE, COLOR_BLACK);
}