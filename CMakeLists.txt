cmake_minimum_required(VERSION 3.19)
project(System_Programming)

set(CMAKE_CXX_STANDARD 17)

file(GLOB lib_SRC
        "*.h"
        )

add_executable(main_Lab_1 Lab1/main.cpp)
add_executable(main_Lab_2 Lab2/mainLab2.cpp Lab2/src/duration_logger.cpp ${lib_SRC})
add_executable(main_Lab_3 Lab3/mainLab3.cpp Lab3/src/duration_logger.cpp ${lib_SRC} Lab3/src/process_tree.cpp)

add_executable(main_Lab_4 Lab4/mainLab4.cpp)
add_executable(main_Lab_5 Lab5/mainLab5.cpp)
add_executable(main_Lab_6 Lab6/mainLab6.cpp)

target_link_libraries(main_Lab_5 PUBLIC pthread)




