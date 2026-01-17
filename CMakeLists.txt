cmake_minimum_required(VERSION 4.0)
project(SourceDiff C)

set(CMAKE_C_STANDARD 11 )
set(CMAKE_SRC_DIR    src)

# TCC

# Sources
file(GLOB SD_SOURCE
    CONFIGURE_DEPENDS
    ${CMAKE_SRC_DIR}/*.c
    ${CMAKE_SRC_DIR}/*.h
)

# Executable
add_executable(SourceDiff ${SD_SOURCE})
