//
// Created by 16905 on 25-7-22.
//

#ifndef BALL_H
#define BALL_H
#include "paddle.h"

#define BALL_STR "◉"
#define BALL_CLEANER " "

struct Ball {
    int x;
    int y;
    int dx;
    int dy;
    struct Paddle *belong_to_paddle;
};

void init_balls();
void clear_ball(struct Ball *);
void print_ball(struct Ball *);
void update_ball(struct Ball *,int);
void clean_balls();

extern struct Ball *ball1;
extern struct Ball *ball2;

#endif //BALL_H
