//
// Created by 16905 on 25-7-20.
//

#ifndef KEY_H
#define KEY_H

#include "pthread.h"

#define KEY_ESC 27
#define LEFT 37
#define RIGHT 39
#define KEY_SPACE 32

#define KEY_1 49
#define KEY_2 50
#define KEY_3 51

void init_listener(void);
void clean_listener(void);

/**
 * 全局变量，用于键盘监听系统中储存用户输入
 */
extern volatile int key_pressed;  // 使用volatile确保线程安全
extern pthread_mutex_t key_mutex;
#endif //KEY_H
