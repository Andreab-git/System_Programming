cmake_minimum_required(VERSION 3.19)
project(System_Programming)

set(CMAKE_CXX_STANDARD 20)

file(GLOB lib_SRC
        "*.h"
        )


add_executable(main_Lab_1 Lab1/main.cpp)
add_executable(main_Lab_2 Lab2/mainLab2.cpp Lab2/src/duration_logger.cpp ${lib_SRC})
add_executable(main_Lab_3 Lab3/mainLab3.cpp Lab3/src/duration_logger.cpp ${lib_SRC}
        Lab3/lib/my_shared.h Lab3/src/process_tree.cpp Lab3/lib/process_tree.h)



