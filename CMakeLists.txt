cmake_minimum_required(VERSION 2.8)
project(sxemu)

SET(CURSES_NEED_NCURSES TRUE)
FIND_PACKAGE(Curses REQUIRED )
include_directories(${CURSES_INCLUDE_DIR})

FIND_PACKAGE ( Threads REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    cpu.cpp
    cpu.h
    filehandler.cpp
    filehandler.h
    hex_file.cpp
    hex_file.h
    sxemu.cpp
    sxkey.h)

add_executable(sxemu ${SOURCE_FILES})
target_link_libraries(sxemu ${CURSES_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
