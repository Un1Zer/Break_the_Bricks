cmake_minimum_required(VERSION 3.20)
project(Break_the_Bricks C)

set(CMAKE_C_STANDARD 11)

# 这三行放在find_package前面，以期解决中文乱码问题
SET(NCURSES_LIBRARY_NAME "ncurses")
set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)

# 寻找curses的库
find_package(Curses REQUIRED)
# 包含curses的库
include_directories(${CURSES_INCLUDE_DIR})
# 包含项目的.h
include_directories(${PROJECT_SOURCE_DIR}/include)


# 拷贝资源文件
FILE(COPY res DESTINATION "${CMAKE_BINARY_DIR}")

# 搜索src目录下的所有文件，赋值给变量SRC_LIST
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)
# 添加库文件，把src生成静态链接库
add_library(SOURCE_LIB STATIC main.c ${SRC_LIST}
        src/bricks.c
        src/ball.c)

# 使用pthread
set(CMAKE_C_FLAGS -pthread)
set(CMAKE_C_STANDARD 11)

add_executable(Break_the_Bricks main.c
        include/bricks.h
        include/ball.h)

# link其他库，包括上面定义的Source_LIB,m-数学库, curses库
target_link_libraries(Break_the_Bricks ${CURSES_LIBRARIES} SOURCE_LIB m)
