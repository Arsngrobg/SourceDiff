cmake_minimum_required(VERSION 4.0)
project(Iridium)

set(CMAKE_C_STANDARD 11)

add_executable(Iridium
        languages/C/parser.c
        src/api.h
        src/main.c
)
