//
// Created by 16905 on 25-7-22.
//
#include "ball.h"

#include <ncurses.h>

#include "colors.h"
#include "game.h"
#include <stdlib.h>

struct Ball *ball1 = NULL;
struct Ball *ball2 = NULL;

void generate_random_ball_position_and_velocity(struct Ball *ball);

/**
 * 为ball指针分配内存，初始化ball的属性
 */
void init_balls() {
    ball1 = (struct Ball*)malloc(sizeof(struct Ball));
    ball2 = (struct Ball*)malloc(sizeof(struct Ball));
    if (ball1 == NULL || ball2 == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    ball1->belong_to_paddle = paddle1;
    ball2->belong_to_paddle = paddle2;

    // 随机化位置和速度
    generate_random_ball_position_and_velocity(ball1);

    ball2->dx = 1;
    ball2->dy = 1;
    ball2->x = ball2->belong_to_paddle->x;
    ball2->y = ball2->belong_to_paddle->y + 4;
}

/**
 * 释放ball占用的内存
 */
void clean_balls() {
    free(ball1);
    ball1 = NULL;
    free(ball2);
    ball2 = NULL;
}

/**
 * 清除旧位置的小球
 * @param b
 */
void clear_ball(struct Ball *b) {
    mvprintw(b->y, b->x, BALL_CLEANER);
}

/**
 * 打印小球
 * @param b
 */
void print_ball(struct Ball *b) {
    attron(COLOR_PAIR(b->belong_to_paddle->belong_to*100));
    mvprintw(b->y, b->x, BALL_STR);
    attroff(COLOR_PAIR(b->belong_to_paddle->belong_to*100));
}

/**
 * 接收Ball*型指针与Effect结构体变量，对指针指向的ball中的Paddle*型指针成员指向的paddle施加相应effect
 * @param b
 * @param effect
 */
void take_effect(struct Ball *b,struct Effect effect) {
    b->belong_to_paddle->score += effect.score_change;
    b->belong_to_paddle->health += effect.health_change;
    b->belong_to_paddle->speed += effect.speed_change;
}

/**
 * 根据游戏模式，对传入的小球进行碰撞检测（包括墙壁、挡板、砖块的碰撞）
 * 注意打到四角的问题
 * @param b
 * @param game_mode
 */
void collision_manage(struct Ball *b,int game_mode) {
    // 撞墙壁 // 撞挡板
    if (game_mode == 1) {
        // 出界
        if (b->y >= b->belong_to_paddle->y) {
            take_effect(b, (struct Effect){0,0,-1});
            return;
        }
        // 撞墙
        if (b->x <= 2 || b->x >= COLS-2) {
            b->dx *= -1;
            // 墙角问题
            if (b->y <= 1) {
                b->dy *= -1;
                return;
            }
        }
        if (b->y <= 1) {
            b->dy *= -1;
            return;
        }
        // 撞板 - 修复：检测小球是否即将撞到挡板
        if (b->x >= b->belong_to_paddle->x-1 && b->x <= b->belong_to_paddle->x+PADDLE_LEN+1) {
            // 检测小球是否从上方接近挡板
            if (b->dy > 0 && b->y < b->belong_to_paddle->y && b->y + b->dy >= b->belong_to_paddle->y - 1) {
                b->dy *= -1;
                // 四角问题
                if (b->x == b->belong_to_paddle->x-1 || b->x == b->belong_to_paddle->x+PADDLE_LEN) {
                    b->dx *= -1;
                }
                return;
            }
        }

    }else if (game_mode == 2 || game_mode == 3) {
        // 出界
        if (b->y >= paddle1->y || b->y <= paddle2->y) {
            take_effect(b, (struct Effect){0,0,-1});
            return;
        }
        // 撞墙
        if (b->x<=2||b->x>=COLS-2) {
            b->dx *= -1;
            return;
        }

        // 修复paddle1碰撞检测
        if (b->x >= paddle1->x-1 && b->x <= paddle1->x+PADDLE_LEN+1) {
            // 检测小球是否从上方接近paddle1
            if (b->dy > 0 && b->y < paddle1->y && b->y + b->dy >= paddle1->y - 1) {
                b->dy *= -1;
                // 四角问题
                if (b->x == paddle1->x-1 || b->x == paddle1->x+PADDLE_LEN+1) {
                    b->dx *= -1;
                }
                return;
            }
        }

        // 修复paddle2碰撞检测
        if (b->x >= paddle2->x-1 && b->x <= paddle2->x+PADDLE_LEN+1) {
            // 检测小球是否从下方接近paddle2
            if (b->dy < 0 && b->y > paddle2->y && b->y + b->dy <= paddle2->y + 1) {
                b->dy *= -1;
                // 四角问题
                if (b->x == paddle2->x-1 || b->x == paddle2->x+PADDLE_LEN+1) {
                    b->dx *= -1;
                }
                return;
            }
        }
    }

    //撞砖块
    struct Collision_Effect ce = check_bricks(b->x, b->y,b->dx,b->dy);
    b->dx *= ce.ddx;
    b->dy *= ce.ddy;
    take_effect(b,ce.effects[0]);
    take_effect(b,ce.effects[1]);
}

/**
 * 根据小球的速度以及碰撞检测结果更新小球的位置与速度
 * @param b
 * @param game_mode
 */
void update_ball(struct Ball *b ,int game_mode) {
    b->x += b->dx;
    b->y += b->dy;
    collision_manage(b,game_mode);
}

/**
 * 为小球生成随机的初始位置与速度
 * @param ball
 */
void generate_random_ball_position_and_velocity(struct Ball *ball) {
    // ball1属于paddle1（下方挡板）
    if (game_mode == SINGLE_MODE) {
        // 单人模式：球在下半区域随机生成
        int safe_left = 10;
        int safe_right = COLS - 10;
        int safe_top = LINES/2 + 5;  // 下半区域
        int safe_bottom = paddle1->y - 5;

        ball->x = safe_left + rand() % (safe_right - safe_left);
        ball->y = safe_top + rand() % (safe_bottom - safe_top);

        // 速度：水平随机，垂直向上（朝砖块）
        ball->dx = (rand() % 2 == 0) ? 1 : -1;
        ball->dy = -1;  // 向上

    } else {  // BATTLE_MODE
        // 对战模式：ball1在paddle1附近生成
        int paddle_center = paddle1->x + PADDLE_LEN / 2;
        int spread = 20;  // 在挡板附近20个字符范围内

        ball->x = paddle_center - spread/2 + rand() % spread;
        ball->y = paddle1->y - 3 - rand() % 3;  // 在挡板上方2-4个位置

        // 确保不超出边界
        if (ball->x < 3) ball->x = 3;
        if (ball->x > COLS - 3) ball->x = COLS - 3;

        // 速度：朝向对方（向上）
        ball->dx = (rand() % 2 == 0) ? 1 : -1;
        ball->dy = -1;  // 向上朝paddle2
    }
}