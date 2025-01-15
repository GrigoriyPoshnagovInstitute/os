cmake_minimum_required(VERSION 3.22.1)
project(10_2_lib)

set(CMAKE_CXX_STANDARD 20)

add_library(10_2_lib SHARED library.cpp
        main.cpp
        main.h)
