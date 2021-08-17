cmake_minimum_required(VERSION 3.16)
project(code C)

set(CMAKE_C_STANDARD 99)

add_executable(code main.c  vect.h watchtower.h read_input.h read_input.c print_to_file.h print_to_file.c
        dcel_struct.h dcel.h dcel.c )