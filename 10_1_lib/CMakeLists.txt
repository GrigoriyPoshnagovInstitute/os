cmake_minimum_required(VERSION 3.26)
project(10_1_lib)

set(CMAKE_CXX_STANDARD 20)

add_library(10_1_lib SHARED quadratic_solver.cpp)
