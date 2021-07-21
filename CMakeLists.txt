cmake_minimum_required(VERSION 3.19)
project(System_Programming)

set(CMAKE_CXX_STANDARD 20)

file(GLOB lib_SRC
        "*.h"
        )


add_executable(main_Lab_1 Lab1/main.cpp)
add_executable(main_Lab_2 Lab2/mainLab2.cpp Lab2/src/DurationLogger.cpp ${lib_SRC})
add_executable(main_Lab_3 Lab3/mainLab3.cpp ${lib_SRC})



