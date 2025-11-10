cmake_minimum_required(VERSION 4.0)
project(Iridium)

set(CMAKE_C_STANDARD 11)

include_directories("./include" "./vendor/tree-sitter/lib/include" "./vendor/tree-sitter/lib/src")

add_executable(Iridium
        vendor/tree-sitter/lib/src/lib.c
        languages/C/parser.c
        src/main.c
)
