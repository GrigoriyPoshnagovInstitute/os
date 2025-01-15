cmake_minimum_required(VERSION 3.26)
project(lab5_task2)

set(CMAKE_CXX_STANDARD 23)

add_executable(lab5_task2 main.cpp
        tests.cpp
        BgThreadDispatcher.h)
