cmake_minimum_required(VERSION 3.28.1)
project(Compila_Project)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Compila_Project
        token.cpp
        scanner.cpp
        main.cpp
        exp.cpp
        parser.cpp
        visitor.cpp
        imp_value.cpp
        environment.h
        type_visitor.cpp)
