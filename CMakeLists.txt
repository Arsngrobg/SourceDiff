cmake_minimum_required(VERSION 4.0)
project(SourceDiff C)

set(CMAKE_C_STANDARD 11)

# easy-args
include_directories(
        vendor/easy-args/includes
)
# tree-sitter
include_directories(
        vendor/tree-sitter/lib/include
        vendor/tree-sitter/lib/src
)
# tinycc
include_directories(
        vendor/tcc
)

# globs
file(GLOB SOURCES src/*.c src/*.h)

add_executable(SourceDiff
        vendor/tree-sitter/lib/src/lib.c
        ${SOURCES}
)
