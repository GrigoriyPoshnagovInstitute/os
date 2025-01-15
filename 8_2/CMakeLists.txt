cmake_minimum_required(VERSION 3.22.1)
project(8_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(8_2 main.cpp)

find_package(X11 REQUIRED)

if (X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
    message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES: ${X11_LIBRARIES}")
else ()
    message(FATAL_ERROR "X11 library not found")
endif ()

# Имя_исполняемого_проекта, которое вы добавляли через add_executable:
target_link_libraries(8_2 ${X11_LIBRARIES})