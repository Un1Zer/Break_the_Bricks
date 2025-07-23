//
// Created by 16905 on 25-7-20.
//
#include <stdlib.h>

#include "ncurses.h"
#include "paddle.h"

#include <pthread.h>

#include "ball.h"
#include "game.h"
#include "key.h"

struct Paddle *paddle1 = NULL;
struct Paddle *paddle2 = NULL;

/**
 * 为paddle指针分配内存，初始化paddle的属性
 */
 void init_paddles() {
     paddle1 = (struct Paddle*)malloc(sizeof(struct Paddle));
     paddle2 = (struct Paddle*)malloc(sizeof(struct Paddle));

    if (paddle1 == NULL || paddle2 == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    paddle1->belong_to = BELONG_TO_PLAYER;
    paddle1->health = 3;
    paddle1->score = 0;
    paddle1->x = 65;
    paddle1->y = 23;
    paddle1->speed = 3;
    paddle2->belong_to = BELONG_TO_COMPUTER;
    paddle2->health = 3;
    paddle2->score = 0;
    paddle2->x = 65;
    paddle2->y = 1;
    paddle2->speed = 3;
}

/**
 * 释放paddle占用的内存
 */
void clean_paddles() {
    free(paddle1);
    paddle1 = NULL;
    free(paddle2);
    paddle2 = NULL;
}

/**
 * 以Paddle所在坐标为起点打印Paddle
 * 原理：调用mvprintw()在指定坐标打印PADDLE_STR
*/
void print_paddle(struct Paddle *p) {
    attron(COLOR_PAIR(p->belong_to*100+1));
    mvprintw(p->y, p->x, PADDLE_STR);
    attroff(COLOR_PAIR(p->belong_to*100+1));
}

/**
 * 清除上次所在坐标绘制的Paddle
 * 原理：调用mvprintw()打印空白字符串覆盖原本打印的PADDLE_STR
 * 好处：不必每次调用clear()或erase()清除整个窗口，仅清除Paddle本身，可避免闪烁
 */
void clear_paddle(struct Paddle *p) {
    mvprintw(p->y, p->x, PADDLE_CLEANER);
}

/**
 * 检查Paddle是否超出游戏界面边界，若是则将其坐标置回原位置
 * @param p
 */
void check_paddle_boundary(struct Paddle *p) {
    if (p->x < 1) {
        p->x = 1;
    }else if (p->x + PADDLE_LEN > COLS - 1) {
        p->x = COLS - 1 -PADDLE_LEN;
    }
}

/**
 * 根据键盘监听结果更新玩家操控的paddle坐标
 * 原理：将全局变量key_pressed的值与有效键盘值进行比对
 * 好处：不在键盘监听器中直接修改Paddle坐标，而是记录键盘值并在Paddle里另写一个更新坐标的方法，增强封装性
 * @param p
 */
void update_paddle_player(struct Paddle *p) {
     // 调试时用过的，纪念一下
     // printw("the key you pressed is: %c, x=%d, y=%d", key_pressed, p->x, p->y);

     pthread_mutex_lock(&key_mutex);
     int current_key = key_pressed;
     key_pressed = 0;  // 清除按键
     pthread_mutex_unlock(&key_mutex);

     switch (current_key) {
        case 'a':case 'A':
            p->x -= p->speed;
            check_paddle_boundary(p);
            break;
        case 'd':case 'D':
            p->x += p->speed;
            check_paddle_boundary(p);
            break;
    }
}

/**
 * BallPrediction型结构体储存了对小球预测的推演信息
 * 包含经过steps_to_paddle步后可能到达的(future_x,future_y)位置
 */
typedef struct {
    int future_x;
    int future_y;
    int steps_to_paddle;
    int is_dangerous;  // 即运动到(future_x,future_y)时是否朝向paddle
} BallPrediction;

/**
 * 推演小球未来50步的动向，预测结果储存在prediction对象里
 * @param ball
 * @param paddle
 * @return 预测结果
 */
BallPrediction predict_ball_position(struct Ball *ball, struct Paddle *paddle) {
    BallPrediction prediction = {0, 0, 1000, 0};

    if (!ball || !paddle) return prediction;

    int current_x = ball->x;
    int current_y = ball->y;
    int dx = ball->dx;
    int dy = ball->dy;
    int steps = 0;

    // 模拟球的运动，最多预测50步
    while (steps < 50) {
        current_x += dx;
        current_y += dy;
        steps++;

        // 模拟遇到墙壁反弹
        if (current_x <= 2 || current_x >= COLS - 3) {
            dx *= -1;
        }

        // 根据挡板位置调整检测范围
        int detection_range = 2;
        bool in_detection_zone = false;

        if (paddle->y <= 5) {  // 在上方（y小）的是paddle2
            in_detection_zone = (current_y <= paddle->y + detection_range);
        } else {  // paddle1在下方
            in_detection_zone = (current_y >= paddle->y - detection_range);
        }

        if (in_detection_zone) {
            prediction.future_x = current_x;
            prediction.future_y = current_y;
            prediction.steps_to_paddle = steps;
            // 球向着板的方向运动才危险
            prediction.is_dangerous = (dy * (ball->y - paddle->y) < 0);
            break;
        }

        // 球明显远离且不可能回来时退出
        bool moving_away = (dy * (ball->y - paddle->y) > 0); // 即is_dangerous反
        bool too_far = false;

        if (paddle->y <= 5) {  // 在上方的是paddle2
            too_far = (current_y > ball->y + 15 && dy > 0);  // 球向下且很远
        } else {  // paddle1在下方
            too_far = (current_y < ball->y - 15 && dy < 0);  // 球向上且很远
        }

        if (moving_away && too_far) {
            break;
        }

    }

    return prediction;
}

/**
 * 根据预测的小球轨迹威胁程度
 * @param p
 */
void update_paddle_computer(struct Paddle *p) {
    // 计算两个球到paddle2的威胁程度
    BallPrediction prediction1 = predict_ball_position(ball1, p);
    BallPrediction prediction2 = predict_ball_position(ball2, p);

    // 选择更危险的球作为目标
    BallPrediction target_prediction;
    int target_x;

    if (prediction1.is_dangerous && prediction2.is_dangerous) {
        // 两个球都危险，选择更近的
        target_prediction = (prediction1.steps_to_paddle < prediction2.steps_to_paddle) ? prediction1 : prediction2;
        target_x = target_prediction.future_x;
    } else if (prediction1.is_dangerous) {
        target_prediction = prediction1;
        target_x = target_prediction.future_x;
    } else if (prediction2.is_dangerous) {
        target_prediction = prediction2;
        target_x = target_prediction.future_x;
    } else {
        // 都不危险，直接跟踪目前最近的球的当前位置即b->x
        int dist1 = abs(ball1->x - p->x) + 2 * abs(ball1->y - p->y);
        int dist2 = abs(ball2->x - p->x) + 2 * abs(ball2->y - p->y);
        target_x = (dist1 < dist2) ? ball1->x : ball2->x;
    }

    // 简单移动逻辑
    int center_x = p->x + PADDLE_LEN / 2; // 板的中心位置

    if (center_x < target_x) {
        p->x += p->speed;
    } else if (p->x + PADDLE_LEN / 2 > target_x) {
        p->x -= p->speed;
    }

    check_paddle_boundary(p);
}

/**
 * 根据传入的paddle对象belong_to属性值判断它是由玩家还是电脑操控
 * 并调用相应的update_paddle函数来更新其坐标
 * @param p
 */
void update_paddle(struct Paddle *p) {
    if (p->belong_to == BELONG_TO_PLAYER) update_paddle_player(p);
    else if (p->belong_to == BELONG_TO_COMPUTER) update_paddle_computer(p);
}