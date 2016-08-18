cmake_minimum_required(VERSION 3.5)
project(Assignment_1 C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -std=c99")

set(SOURCE_FILES pong.c)
add_library(zdk)
add_library(ncurses)
set_target_properties(zdk PROPERTIES LINKER_LANGUAGE C)
set_target_properties(ncurses PROPERTIES LINKER_LANGUAGE C)

add_executable(Assignment_1 ${SOURCE_FILES})
target_link_libraries(Assignment_1 zdk)
target_link_libraries(Assignment_1 nurses)