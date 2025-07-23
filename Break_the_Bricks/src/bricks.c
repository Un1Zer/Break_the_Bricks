//
// Created by 16905 on 25-7-21.
//
#include "curses.h"
#include "bricks.h"

#include <stdlib.h>
#include <unistd.h>

#include "game.h"

// brick_types是记录所有砖块类型的整型二维数组，当其中某一个元素值为0时表示此砖块已不复存在
int brick_types[BRICKS_ROW][BRICKS_COL];
// bricks是一个Brick结构体型二维数组，其中所有元素与brick_types数组中的元素一一对应
struct Brick bricks[BRICKS_ROW][BRICKS_COL];
// effects是Effect结构体型一维数组，以砖块种类宏定义为索引
struct Effect effects[6] =
    {{0,0,0},{1,0,0},
{2,0,0},{0,0,-1},
    {0,1,0},{0,0,1}
    };
// appearances是字符串型二维数组，row以砖块种类宏定义为索引，col以该类砖块的health属性减去1后为索引
char* appearances[6][3] =
    {{NULL,NULL,NULL},{NORMAL_BRICK_BROKEN,NORMAL_BRICK_HURT,NORMAL_BRICK_COMPLETE},
    {PRECIOUS_BRICK_BROKEN,PRECIOUS_BRICK_COMPLETE,NULL},{HURT_BRICK_BROKEN,HURT_BRICK_COMPLETE,NULL},
    {SPEED_BRICK_BROKEN,SPEED_BRICK_COMPLETE,NULL},{HELP_BRICK_BROKEN,HELP_BRICK_COMPLETE,NULL}
    };

void generate_random_bricks(int row, int col, int arr[row][col]);

/**
 * 初始化所有砖块的坐标等属性
 * 原理：先调用generate_random_bricks()初始化储存砖块类型的int型二维数组，根据该随机数组初始化bricks结构体数组
 */
void init_bricks(int game_mode) {
    int start_y = START_Y_SINGLE;
    if (game_mode == BATTLE_MODE || game_mode == MOVIE_MODE) {
        start_y = START_Y_BATTLE;
    }
    generate_random_bricks(BRICKS_ROW ,BRICKS_COL, brick_types);
    for (int i = 0; i < BRICKS_ROW; i++) {
        for (int j = 0; j < BRICKS_COL; j++) {
            switch (brick_types[i][j]) {
                case NORMAL_BRICK :
                    bricks[i][j] = (struct Brick)
                    {j*BRICK_WIDTH+START_X,i*BRICK_HEIGHT+start_y,3,appearances[NORMAL_BRICK],
                        BOOM_MESSAGE,effects[NORMAL_BRICK]};
                    break;
                case PRECIOUS_BRICK:
                    bricks[i][j] = (struct Brick)
                    {j*BRICK_WIDTH+START_X,i*BRICK_HEIGHT+start_y,2,appearances[PRECIOUS_BRICK],
                        YEAH_MESSAGE,effects[PRECIOUS_BRICK]};
                    break;
                case HURT_BRICK:
                    bricks[i][j] = (struct Brick)
                    {j*BRICK_WIDTH+START_X,i*BRICK_HEIGHT+start_y,2,appearances[HURT_BRICK],
                        OOPS_MESSAGE,effects[HURT_BRICK]};
                    break;
                case SPEED_BRICK:
                    bricks[i][j] = (struct Brick)
                    {j*BRICK_WIDTH+START_X,i*BRICK_HEIGHT+start_y,2,appearances[SPEED_BRICK],
                        DASH_MESSAGE,effects[SPEED_BRICK]};
                    break;
                case HELP_BRICK:
                    bricks[i][j] = (struct Brick)
                    {j*BRICK_WIDTH+START_X,i*BRICK_HEIGHT+start_y,2,appearances[HELP_BRICK],
                        HELP_MESSAGE,effects[HELP_BRICK]};
                    break;
            }
        }
    }
}

/**
 * 打印所有health>0的砖块
 */
void print_bricks() {
    int has_bricks = 0;
    for (int i = 0; i < BRICKS_ROW; i++) {
        for (int j = 0; j < BRICKS_COL; j++) {
            if (brick_types[i][j] <= 0 || bricks[i][j].health <= 0) {
                continue;
            }
            attron(COLOR_PAIR(brick_types[i][j]*10 + bricks[i][j].health));
            mvaddstr(bricks[i][j].y, bricks[i][j].x, bricks[i][j].appearance[bricks[i][j].health - 1]);
            attroff(COLOR_PAIR(brick_types[i][j]*10 + bricks[i][j].health));
            has_bricks++;
        }
    }
    if (has_bricks == 0) {game_over = 1;}
}

/**
 * 对于health<=0的砖块打印消除信息,执行砖块效果，并清除砖块
 * 原理：在health<=0的砖块坐标位置先打印相应的消除信息，usleep()后打印空白字符串，随后将其在brick_types数组中对应的值置为0以作标记
 */
void clear_bricks() {
    for (int i = 0; i < BRICKS_ROW; i++) {
        for (int j = 0; j < BRICKS_COL; j++) {
            if (brick_types[i][j] == -1) {
                mvaddstr(bricks[i][j].y,bricks[i][j].x,BRICK_CLEANER);
                brick_types[i][j] = 0;
            }
            if (brick_types[i][j] && bricks[i][j].health <= 0) {
                attron(COLOR_PAIR(brick_types[i][j]*10));
                mvaddstr(bricks[i][j].y,bricks[i][j].x,bricks[i][j].message);
                attroff(COLOR_PAIR(brick_types[i][j]*10));
                brick_types[i][j] = -1;
            }
        }
    }
}

/**
 *传入坐标，检查该坐标是否存在砖块，若有则砖块生命值减少
 *随后判断砖块是否被击破，并根据判断结果返回不同的Effect结构体对象作为标识
 */
struct Collision_Effect check_bricks(int x,int y,int dx,int dy) {
    struct Collision_Effect ce = {{{0,0,0},
        {0,0,0}}, 1, 1};

    int next_x = x + dx;
    int next_y = y + dy;

    for (int i = 0; i < BRICKS_ROW; i++) {
        for (int j = 0; j < BRICKS_COL; j++) {
            if (brick_types[i][j] <= 0 || bricks[i][j].health <= 0) continue;

            int brick_x = bricks[i][j].x;
            int brick_y = bricks[i][j].y;

            // 检查小球下一个位置是否与砖块碰撞
            if (next_x >= brick_x && next_x < brick_x + BRICK_WIDTH &&
                next_y >= brick_y && next_y < brick_y + BRICK_HEIGHT) {

                // 确定碰撞类型
                int hit_from_left = (x < brick_x && next_x >= brick_x);
                int hit_from_right = (x >= brick_x + BRICK_WIDTH && next_x < brick_x + BRICK_WIDTH);
                int hit_from_top = (y < brick_y && next_y >= brick_y);
                int hit_from_bottom = (y >= brick_y + BRICK_HEIGHT && next_y < brick_y + BRICK_HEIGHT);

                // 根据碰撞方向调整反弹
                if (hit_from_left || hit_from_right) {
                    ce.ddx = -1;
                }
                if (hit_from_top || hit_from_bottom) {
                    ce.ddy = -1;
                }

                // 如果是对角碰撞，两个方向都反弹
                if ((hit_from_left || hit_from_right) && (hit_from_top || hit_from_bottom)) {
                    ce.ddx = -1;
                    ce.ddy = -1;
                }

                // 减少砖块生命值并获取效果
                bricks[i][j].health--;
                if (bricks[i][j].health <= 0) {
                    ce.effects[0] = bricks[i][j].e;
                }

                return ce;
                }
        }
    }

    return ce;
}

/**
 * 生成数据满足一定条件的随机二维数组，以实现砖块随机分布的效果
 * 原理：先将数据按照比例要求放入一维数组，再打乱数组后划分并放入二维数组
 * @param row
 * @param col
 * @param arr
 */
void generate_random_bricks(int row, int col, int arr[row][col]) {
    int n = row * col;

    if (n < 2) {
        printw("num of bricks is less than 2");
        return;
    }

    int *flat = malloc(n * sizeof(int));
    if (!flat) {
        printw("malloc failed!");
        return;
    }

    int counts[6] = {0}; // 用于统计 1~5 的数量

    int remaining = n - 2;
    counts[1] = (int)(remaining * 0.6 + 0.5);
    counts[2] = (int)(remaining * 0.2 + 0.5);
    counts[3] = (int)(remaining * 0.1 + 0.5);
    counts[5] = remaining - (counts[1] + counts[2] + counts[3]);

    // 调整误差，确保总数为 n - 2
    int total = counts[1] + counts[2] + counts[3] + counts[5];
    while (total != remaining) {
        if (total > remaining) {
            if (counts[1] > 0) counts[1]--;
            else if (counts[2] > 0) counts[2]--;
            else if (counts[3] > 0) counts[3]--;
            else if (counts[5] > 0) counts[5]--;
        } else {
            counts[1]++;
        }
        total = counts[1] + counts[2] + counts[3] + counts[5];
    }

    // 填充 flat 一维数组
    int idx = 0;
    for (int val = 1; val <= 5; val++) {
        if (val == 4) continue;
        for (int i = 0; i < counts[val]; i++) {
            flat[idx++] = val;
        }
    }
    // 添加两个4
    flat[idx++] = 4;
    flat[idx++] = 4;

    // 打乱 flat（Fisher–Yates 洗牌）
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = flat[i];
        flat[i] = flat[j];
        flat[j] = temp;
    }

    // 将 flat 填入二维数组 arr[row][col]
    idx = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            arr[i][j] = flat[idx++];
        }
    }

    free(flat);
}

/**
 *废弃的check_bricks代码
 */
//     // 难点：墙角问题
//     int up_or_down = 0;
//     int left_or_right = 0;
//     int wall1_row = 0,wall1_col = 0,wall2_row = 0,wall2_col = 0;
//
//     struct Collision_Effect ce = {{{0,0,0},
// {0,0,0}},1,1};
//
//     // 第一次循环专门处理墙角难题
//     for (int i = 0; i < BRICKS_ROW; i++) {
//         for (int j = 0; j < BRICKS_COL; j++) {
//             // 检查有无对角砖块
//             if (brick_types[i][j] && y+dy==bricks[i][j].y &&
//             (x+dx==bricks[i][j].x || x+dx==bricks[i][j].x+BRICK_WIDTH)){
//                 // 检查是否是陷入墙角困局
//                 for (int k = i-1;k <= i+1;k++) {
//                     if (k<0) k++;
//                     for (int l = j-1;l <= j+1;l++) {
//                         if (l<0) l++;
//                         if (brick_types[k][l] && (x+dx==bricks[i][j].x || x+dx==bricks[i][j].x+BRICK_WIDTH) &&
//                             (y+dy==bricks[i][j].y-BRICK_HEIGHT || y+dy==bricks[i][j].y+BRICK_HEIGHT )) {
//                             wall1_row = k;
//                             wall1_col = l;
//                             left_or_right = 1;
//                         }else if (brick_types[k][l] && y+dy==bricks[i][j].y &&
//                             (x+dx==bricks[i][j].x-1 || x+dx==bricks[i][j].x+BRICK_WIDTH+1)) {
//                             wall2_row = k;
//                             wall2_col = l;
//                             up_or_down = 1;
//                         }
//                     }
//                 }
//                 if (up_or_down && left_or_right) {
//                     // 有墙角
//                     ce.ddx = ce.ddy = -1;
//                     if (--bricks[wall1_row][wall1_col].health <= 0) {
//                         ce.effects[0] = bricks[wall1_row][wall1_col].e;
//                     }
//                     if (--bricks[wall2_row][wall2_col].health <= 0) {
//                         ce.effects[1] = bricks[wall2_row][wall2_col].e;
//                     }
//                 }else {
//                     // 无墙角，按打到对角砖块处理
//                     ce.ddx = ce.ddy = -1;
//                     if (--bricks[i][j].health <= 0) {
//                         ce.effects[0] = bricks[i][j].e;
//                     }
//                 }
//                 return ce;
//             }
//         }
//     }
//     for(int i = 0; i < BRICKS_ROW; i++) {
//         for (int j = 0; j < BRICKS_COL; j++) {
//             // 打到上下砖块
//             if (brick_types[i][j] && y+dy==bricks[i][j].y &&
//                 (x+dx>=bricks[i][j].x || x+dx<=bricks[i][j].x+BRICK_WIDTH)) {
//                 ce.ddy = -1;
//                 if (--bricks[i][j].health <= 0) {
//                     ce.effects[0] = bricks[i][j].e;
//                 }
//                 return ce;
//             }
//             // 打到左右砖块
//             if (brick_types[i][j] && y+dy==bricks[i][j].y &&
//                 (x+dx==bricks[i][j].x-1 || x+dx<=bricks[i][j].x+BRICK_WIDTH+1)) {
//                 ce.ddx = -1;
//                 if (--bricks[i][j].health <= 0) {
//                     ce.effects[0] = bricks[i][j].e;
//                 }
//                 return ce;
//             }
//         }
//     }
//     // 没打到
//     return ce;