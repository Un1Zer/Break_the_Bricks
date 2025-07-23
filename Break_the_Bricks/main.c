//
// Created by 16905 on 25-7-20.
//
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <locale.h>

#include "ball.h"
#include "curses.h"
#include "key.h"
#include "game.h"
#include "colors.h"
#include "paddle.h"

#define TITLE_FILENAME "res/title.txt"
#define INSTRUCTIONS_FILENAME "res/instructions.txt"
#define CREDITS_FILENAME "res/credits.txt"
#define MENU_FILENAME "res/menu.txt"

#define QUIT 0
#define STARTING 1

/**
 * 配置curses环境
 */
void configure_curses() {
    setlocale(LC_ALL,""); // 这个函数放在initscr前
    initscr();
    init_colors();
    raw(); // 禁用行缓冲
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // 不显示光标
}

/**
 * 读取文件中字符数量，以此确定储存文件的字符串长度
 * @param fd
 */
int count_file_chars(int fd) {
    // 使用SEEK_END找到文件大小
    ssize_t file_size = lseek(fd, 0, SEEK_END);
    //将文件指针重置到0，以便后面读取文件内容
    lseek(fd, 0, SEEK_SET);
    return file_size;
}

/**
 * 将文件内容转化为字符串并打印在指定坐标
 * @param file_name
 * @param x
 * @param y
 */
void print_file(char* file_name,unsigned int x,unsigned int y) {
    // 1.读取文件内容
    int fd = open(file_name,O_RDONLY);

    if (fd > 0) {
        // 2.将文件内容储存到字符串中
        int file_size = count_file_chars(fd);
        char buffer[file_size + 1]; // +1是因为字符串最后一位是\0
        ssize_t bytes_read = read(fd, buffer, file_size);
        if (bytes_read == -1) {
            perror("Error reading file");
            close(fd);
            endwin();
            exit(1);
        }
        buffer[file_size] = '\0'; // 文件的最后一位置为0，转成字符串

        // 3.打印字符串
        mvprintw(y, x, "%s", buffer); // y即row行数，x即col列数

        if (close(fd) == -1) {
            perror("Error closing file");
            exit(1);
        }
    }else {
        printf("Error opening file\n");
        close(fd);
        endwin();
        exit(1);
    }
}

/**
 * 打印最开始的用户主界面
 */
void print_beginning_interface() {
    // window row is 30,col is 120
    // title width is 78,height is 6
    print_file(TITLE_FILENAME,0,0);
    print_file(INSTRUCTIONS_FILENAME,0,8);
    print_file(CREDITS_FILENAME,0,28);
}

/**
 * 打印第二级菜单界面
 */
void print_menu_interface() {
    print_file(TITLE_FILENAME,0,0);
    print_file(MENU_FILENAME,0,8);
}

/**
 * 用于主界面读取并返回用户选择
 * @return user_choice
 */
int home_get_choice() {
    while (1) {
        char c = getch();
        if (c == KEY_ESC) {
            return QUIT;
        }
        if (c == KEY_SPACE || c == KEY_ENTER || c == '\n') {
            clear();
            print_menu_interface();
            refresh();
            return STARTING;
        }
        usleep(10000);
    }

}

/**
 * 用于菜单界面读取并返回用户选择
 * @return user_choice
 */
int menu_get_choice() {
    while (1) {
        char c = getch();
        if (c == KEY_ESC) {
            return QUIT;
        }
        if (c == KEY_1) {
            clear();
            return SINGLE_MODE;
        }
        if (c == KEY_2) {
            clear();
            return BATTLE_MODE;
        }
        if (c == KEY_3) {
            clear();
            return MOVIE_MODE;
        }
        usleep(10000);
    }
}

/**
 * 主函数入口
 * 包含环境配置与指针变量初始化，以及用户界面相关逻辑
 */
int main(void) {
    // 添加随机数种子初始化
    srand((unsigned int)time(NULL));

    // 环境配置与变量初始化
    configure_curses();

    // 初始化挡板
    init_paddles();
    // 初始化小球
    init_balls();

    // 用户界面打印与选择逻辑
    print_beginning_interface();
    refresh();
    if (home_get_choice()) {
        game_mode = menu_get_choice();
        refresh();
        if (game_mode) {
            clear();
            start_game();
        }
    }

    // 释放内存
    pthread_mutex_destroy(&key_mutex);
    clean_paddles();
    clean_balls();

    endwin();
    return 0;
}

/**
 *废弃代码
 */
// #include <ncurses.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <time.h>
//
// #define DELAY 40000
// #define WIDTH 80
// #define HEIGHT 24
// #define PADDLE_WIDTH 7
// #define BRICK_ROWS 5
// #define BRICK_COLS 10
// #define BRICK_WIDTH 6
// #define BRICK_HEIGHT 2
//
// typedef struct {
//     int x, y;
//     int dx, dy;
// } Ball;
//
// typedef struct {
//     int x;
// } Paddle;
//
// typedef struct {
//     int hp;
//     int color;
// } Brick;
//
// Brick bricks[BRICK_ROWS][BRICK_COLS];
// Paddle Paddle;
// Ball ball;
//
// void init_colors() {
//     start_color();
//     init_pair(1, COLOR_RED, COLOR_BLACK);     // 砖块生命值 3
//     init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // 生命值 2
//     init_pair(3, COLOR_GREEN, COLOR_BLACK);   // 生命值 1
//     init_pair(4, COLOR_CYAN, COLOR_BLACK);    // 球
//     init_pair(5, COLOR_WHITE, COLOR_BLUE);    // 挡板
//     init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // 爆炸
// }
//
// void draw_bricks() {
//     for (int i = 0; i < BRICK_ROWS; ++i) {
//         for (int j = 0; j < BRICK_COLS; ++j) {
//             Brick *b = &bricks[i][j];
//             if (b->hp > 0) {
//                 attron(COLOR_PAIR(b->color));
//                 for (int dx = 0; dx < BRICK_WIDTH; ++dx)
//                     mvaddch(i * BRICK_HEIGHT, j * BRICK_WIDTH + dx, '=');
//                 attroff(COLOR_PAIR(b->color));
//             }
//         }
//     }
// }
//
// void draw_paddle() {
//     attron(COLOR_PAIR(5));
//     for (int i = 0; i < PADDLE_WIDTH; ++i)
//         mvaddch(HEIGHT - 2, Paddle.x + i, '=');
//     attroff(COLOR_PAIR(5));
// }
//
// void draw_ball() {
//     attron(COLOR_PAIR(4));
//     mvaddch(ball.y, ball.x, 'O');
//     attroff(COLOR_PAIR(4));
// }
//
// void boom_Brick(int y, int x) {
//     attron(COLOR_PAIR(6));
//     mvprintw(y, x, "BOOM!");
//     attroff(COLOR_PAIR(6));
//     refresh();
//     usleep(60000);
// }
//
// void move_ball() {
//     // 清除旧球
//     mvaddch(ball.y, ball.x, ' ');
//
//     int new_x = ball.x + ball.dx;
//     int new_y = ball.y + ball.dy;
//
//     // 撞墙
//     if (new_x <= 0 || new_x >= WIDTH - 1)
//         ball.dx *= -1;
//     if (new_y <= 0)
//         ball.dy *= -1;
//
//     // 撞挡板
//     if (new_y == HEIGHT - 3 && new_x >= Paddle.x && new_x <= Paddle.x + PADDLE_WIDTH) {
//         ball.dy *= -1;
//     }
//
//     // 撞砖块
//     for (int i = 0; i < BRICK_ROWS; ++i) {
//         for (int j = 0; j < BRICK_COLS; ++j) {
//             Brick *b = &bricks[i][j];
//             if (b->hp > 0) {
//                 int bx = j * BRICK_WIDTH;
//                 int by = i * BRICK_HEIGHT;
//                 if (new_y == by && new_x >= bx && new_x < bx + BRICK_WIDTH) {
//                     ball.dy *= -1;
//                     b->hp--;
//                     if (b->hp == 2) b->color = 2;
//                     if (b->hp == 1) b->color = 3;
//                     if (b->hp == 0) boom_Brick(by, bx);
//                     return;
//                 }
//             }
//         }
//     }
//
//     // 撞底部：重置
//     if (new_y >= HEIGHT - 1) {
//         ball.x = WIDTH / 2;
//         ball.y = HEIGHT / 2;
//         ball.dx = 1;
//         ball.dy = -1;
//         return;
//     }
//
//     ball.x += ball.dx;
//     ball.y += ball.dy;
// }
//
// void init_game() {
//     // 初始化砖块
//     for (int i = 0; i < BRICK_ROWS; ++i) {
//         for (int j = 0; j < BRICK_COLS; ++j) {
//             bricks[i][j].hp = 3;
//             bricks[i][j].color = 1;
//         }
//     }
//
//     Paddle.x = WIDTH / 2 - PADDLE_WIDTH / 2;
//     ball.x = WIDTH / 2;
//     ball.y = HEIGHT / 2;
//     ball.dx = 1;
//     ball.dy = -1;
// }
//
// int main() {
//     initscr();
//     noecho();
//     cbreak();
//     curs_set(FALSE);
//     nodelay(stdscr, TRUE);
//     keypad(stdscr, TRUE);
//
//     init_colors();
//     init_game();
//
//     int quit = 1;
//     while (quit) {
//         clear();
//         draw_bricks();
//         draw_paddle();
//         draw_ball();
//         refresh();
//
//         int ch = getch();
//         if (ch == KEY_LEFT && Paddle.x > 0) Paddle.x--;
//         if (ch == KEY_RIGHT && Paddle.x + PADDLE_WIDTH < WIDTH) Paddle.x++;
//         if (ch == 27) quit = 0;
//
//         move_ball();
//
//         usleep(DELAY);
//     }
//
//     endwin();
//     return 0;
// }
