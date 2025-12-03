cmake_minimum_required(VERSION 4.0)
project(SourceDiff C)

set(CMAKE_C_STANDARD 11)

# tree-sitter
file(GLOB TREE_SITTER_lib vendor/tree-sitter/lib/src/*.c vendor/tree-sitter/lib/src/*.h)
include_directories(vendor/tree-sitter/lib/include)

add_executable(SourceDiff
        ${TREE_SITTER_lib}
        src/main.c
)
