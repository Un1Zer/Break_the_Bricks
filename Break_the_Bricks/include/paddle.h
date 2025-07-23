//
// Created by 16905 on 25-7-20.
//

#ifndef PADDLE_H
#define PADDLE_H

#define PADDLE_CH '='
#define PADDLE_LEN 10
#define PADDLE_STR "=========="
#define PADDLE_CLEANER "          "

#define BELONG_TO_PLAYER 1
#define BELONG_TO_COMPUTER 2

struct Paddle {
    int belong_to; // 根据belong判断paddle由谁操控，计算对应ball和paddle的颜色对序号
    int health;
    int score;
    int x;
    int y;
    float speed;
};

void init_paddles();
void print_paddle(struct Paddle*);
void clear_paddle(struct Paddle*);
void update_paddle(struct Paddle*);
void clean_paddles();

extern struct Paddle *paddle1;
extern struct Paddle *paddle2;

#endif //PADDLE_H
