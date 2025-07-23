//
// Created by 16905 on 25-7-21.
//

#ifndef BRICKS_H
#define BRICKS_H

#define NORMAL_BRICK 1 // 60percent
#define PRECIOUS_BRICK 2 // 20percent
#define HURT_BRICK 3 // 10 PERCENT
#define SPEED_BRICK 4 // ONLY 2
#define HELP_BRICK 5 // 10 PERCENT

#define BOOM_MESSAGE "BOOM!"
#define YEAH_MESSAGE "YEAH!"
#define OOPS_MESSAGE "OOPS!"
#define HELP_MESSAGE "HELP!"
#define DASH_MESSAGE "DASH!"

#define BRICK_WIDTH 5
#define BRICK_HEIGHT 1

#define BRICK_LEFT "["
#define BRICK_RIGHT "]"
#define NORMAL_BRICK_COMPLETE "[███]"
#define NORMAL_BRICK_HURT "[▓▓▓]"
#define NORMAL_BRICK_BROKEN "[░░░]"
#define PRECIOUS_BRICK_COMPLETE "[$$$]"
#define PRECIOUS_BRICK_BROKEN "[SSS]"
#define HURT_BRICK_COMPLETE "[XXX]"
#define HURT_BRICK_BROKEN "[|||]"
#define SPEED_BRICK_COMPLETE "[+++]"
#define SPEED_BRICK_BROKEN "[---]"
#define HELP_BRICK_COMPLETE "[HHH]"
#define HELP_BRICK_BROKEN "[III]"
#define BRICK_CLEANER "     "

#define ALL_BRICKS_HEIGHT 10
#define ALL_BRICKS_WIDTH 115
#define BRICKS_COL ALL_BRICKS_WIDTH/BRICK_WIDTH
#define BRICKS_ROW ALL_BRICKS_HEIGHT/BRICK_HEIGHT

#define START_Y_SINGLE 1
#define START_Y_BATTLE 7
#define START_X 2

struct Effect {
    int score_change;
    int speed_change;
    int health_change;
};

struct Brick {
    int x;
    int y;
    int health;
    char** appearance;
    char* message;
    struct Effect e;
};

/**
 * 记录检查小球碰撞砖块产生的结果的结构体
 */
struct Collision_Effect {
    struct Effect effects[2];
    int ddx;
    int ddy;
};

void init_bricks(int);
void print_bricks();
void clear_bricks();
struct Collision_Effect check_bricks(int x,int y,int dx,int dy);

#endif //BRICKS_H
