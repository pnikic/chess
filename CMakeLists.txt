cmake_minimum_required(VERSION 3.10)
project(ChApp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

include_directories(base gui)

find_package(Qt5Widgets)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

add_executable(ChApp WIN32
    gui/chessboardscene.cpp
    gui/main.cpp
    gui/mainwindow.cpp
    base/base.cpp
    base/board.cpp
    base/square.cpp
    base/move.cpp
    base/piece.cpp
    base/setsqr.cpp
)

target_link_libraries(ChApp Qt5::Widgets)
