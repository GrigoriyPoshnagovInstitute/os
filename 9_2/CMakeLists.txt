cmake_minimum_required(VERSION 3.26)
project(9_2)

set(BOOST_ROOT "C:/Users/Gregogo/Downloads/boost_1_87_0")

include_directories("C:/Users/Gregogo/Downloads/boost_1_87_0")
# Указываем какие компоненты Boost использовать (например, только Asio)
#find_package(Boost 1.87.0 REQUIRED COMPONENTS asio)

# Указываем директории для включения заголовочных файлов Boost
include_directories(${Boost_INCLUDE_DIRS})

# Указываем библиотеки Boost (если они необходимы)
link_directories(${Boost_LIBRARY_DIRS})

set(CMAKE_CXX_STANDARD 20)

add_executable(9_2 main.cpp)