cmake_minimum_required(VERSION 3.5)
project(Assignment_1 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -D_XOPEN_SOURCE=500 -Wall -Werror -lncurses -lm")

set(SOURCE_FILES makefile pong.c)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ZDK/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ZDK/)

find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})

add_executable(Assignment_1 ${SOURCE_FILES})

target_link_libraries(Assignment_1 zdk ${CURSES_LIBRARIES})