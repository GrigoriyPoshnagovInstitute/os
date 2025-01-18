cmake_minimum_required(VERSION 3.16)

project(Whiteboard)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем пути к Qt и Boost
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.1/mingw_64/lib/cmake;C:/vs/vcpkg/installed/x64-windows/share/boost")

# Ищем Boost
find_package(Boost REQUIRED COMPONENTS system headers)

# Ищем Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Источники
set(SOURCES
        main.cpp
        WhiteboardApp.cpp
        WhiteboardApp.h
        Server.cpp
        Server.h
        Client.cpp
        Client.h
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Boost::system Boost::headers)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network Boost::system)
target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
target_link_libraries(Whiteboard PRIVATE ws2_32 mswsock)