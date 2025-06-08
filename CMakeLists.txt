cmake_minimum_required(VERSION 3.29)
project(Compila_Project)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Compila_Project
        token.cpp
        scanner.cpp
        main.cpp)
