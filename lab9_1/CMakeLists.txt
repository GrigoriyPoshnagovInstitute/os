cmake_minimum_required(VERSION 3.26)
project(lab9_1)

set(CMAKE_CXX_STANDARD 20)

# Сначала создаём исполнимый файл
add_executable(lab9_1 main.cpp)

# Затем линкуем с библиотекой
target_link_libraries(lab9_1 PRIVATE ws2_32)