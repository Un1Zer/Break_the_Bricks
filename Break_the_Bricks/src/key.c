//
// Created by 16905 on 25-7-20.
//
#include <stdlib.h>
#include "ncurses.h"
#include "key.h"

#include <unistd.h>

#include "game.h"
#include "paddle.h"
#include "pthread.h"

volatile int key_pressed = 0;
pthread_mutex_t key_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * 读取并记录用户输入，处理退出操作
 * 原理：通过getch()将用户输入储存在全局变量key_pressed中
 * 且在key_pressed与ESC值相等时置paddle.health为负值以作标识，随即结束自我线程
 */
void *key_listener(void *arg) {
    while (paddle1->health > 0 && paddle2->health > 0) {
        int ch = getch();
        if (ch != ERR) {  // ERR表示没有输入
            pthread_mutex_lock(&key_mutex);
            key_pressed = ch;
            pthread_mutex_unlock(&key_mutex);
            if (key_pressed == KEY_ESC) {
                game_over = -1;
            }
        }
        usleep(10000);  // 短暂延迟，减少CPU占用
    }

}


/**
 * 初始化键盘监听器，使用户可以实时控制Paddle移动
 * 原理：为key_listener()函数创建一个子线程与主线程同步运行
 */
void init_listener(void) {
    // 设置非阻塞输入
    nodelay(stdscr, TRUE);
    int rc;
    pthread_t tid;
    rc = pthread_create(&tid, NULL, &key_listener, NULL);
    if (rc) {
        fprintf(stderr, "cant create new thread! error code %d", rc);
        endwin();
        exit(1);
    }

    // 设置线程为分离状态，自动清理资源
    pthread_detach(tid);
}

/**
 *废弃的多线程键盘监听代码，ncurses非线程安全
 */
// // 创建锁保护Paddle坐标
// pthread_mutex_t Paddle_positon_mutex_1;
// pthread_mutex_t Paddle_positon_mutex_2;
//
// // 监听键盘输入，更改Paddle坐标，同时监听ESC退出
// void *key_listener_1(void *arg) {
//     while (paddle1.health > 0) {
//         char ch = getch();
//         if (ch == 'A' || ch == 'a') {
//             pthread_mutex_lock(&Paddle_positon_mutex_1);
//             paddle1.x -= paddle1.speed;
//             pthread_mutex_unlock(&Paddle_positon_mutex_1);
//         } else if (ch == 'D' || ch == 'd') {
//             pthread_mutex_lock(&Paddle_positon_mutex_1);
//             paddle1.x += paddle1.speed;
//             pthread_mutex_unlock(&Paddle_positon_mutex_1);
//         } else if (ch == KEY_ESC) {
//             // 结算
//             paddle1.health = -1;
//             endwin();
//             pthread_mutex_destroy(&Paddle_positon_mutex_1);
//             pthread_exit(NULL);
//         }
//     }
//     pthread_mutex_destroy(&Paddle_positon_mutex_1);
//     pthread_exit(NULL);
// }
//
// void *key_listener_2(void *arg) {
//     while (paddle2.health > 0) {
//         char ch = getch();
//         if (ch == KEY_LEFT) {
//             pthread_mutex_lock(&Paddle_positon_mutex_2);
//             paddle2.x -= paddle2.speed;
//             pthread_mutex_unlock(&Paddle_positon_mutex_2);
//         } else if (ch == KEY_RIGHT) {
//             pthread_mutex_lock(&Paddle_positon_mutex_2);
//             paddle2.x += paddle2.speed;
//             pthread_mutex_unlock(&Paddle_positon_mutex_2);
//         }else if (ch == KEY_ESC) {
//             paddle2.health = -1;
//             endwin();
//             pthread_mutex_destroy(&Paddle_positon_mutex_2);
//             pthread_exit(NULL);
//         }
//     }
//     pthread_mutex_destroy(&Paddle_positon_mutex_2);
//     pthread_exit(NULL);
// }
//
// // 初始化单人/对战模式键盘监听器，创建一个线程
// void init_single_listener(void) {
//     int rc;
//     pthread_t tid;
//     // 初始化互斥锁
//     pthread_mutex_init(&Paddle_positon_mutex_1, NULL);
//     // 为键盘监听器创建一个线程
//     rc = pthread_create(&tid, NULL, &key_listener_1, NULL);
//     if (rc) {
//         fprintf(stderr, "cant create new thread! error code %d", rc);
//         endwin();
//         exit(1);
//     }
// }
//
// // 初始化多人对战键盘监听器，创建两个线程
// void init_multiplay_listener(void) {
//     int rc1,rc2;
//     pthread_t tid1;
//     pthread_t tid2;
//     pthread_mutex_init(&Paddle_positon_mutex_1, NULL);
//     pthread_mutex_init(&Paddle_positon_mutex_2, NULL);
//     // 为两个玩家的键盘监听器分别创建线程
//     rc1 = pthread_create(&tid1, NULL, &key_listener_1, NULL);
//     rc2 = pthread_create(&tid2, NULL, &key_listener_2, NULL);
//     if (rc1 || rc2) {
//         if (rc1) fprintf(stderr, "cant create new thread! error code %d", rc1);
//         if (rc2) fprintf(stderr, "cant create new thread! error code %d", rc2);
//         endwin();
//         exit(1);
//     }
// }