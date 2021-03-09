cmake_minimum_required(VERSION 3.19)
project(lab4 C)

set(CMAKE_C_STANDARD 11)

find_package(X11 REQUIRED)
message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lX11")

add_executable(lab4 main.c)