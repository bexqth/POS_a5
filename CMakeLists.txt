cmake_minimum_required(VERSION 3.18)
project(uloha5 C)

set(CMAKE_C_STANDARD 11)

add_executable(uloha5 main.c)
target_link_libraries(uloha5 pthread)
