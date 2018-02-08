cmake_minimum_required(VERSION 3.9)
project(ECEN390 C)

set(CMAKE_C_STANDARD 99)

add_executable(ECEN390 main.c queue.c filter.c filter.h histogram.c histogram.h filterTest.c filterTest.h supportFiles/utils.cpp)
