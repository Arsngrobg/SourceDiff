cmake_minimum_required(VERSION 4.0)
project(SourceDiff C)

set(CMAKE_C_STANDARD 11        )
set(SRC_DIR          src       )
set(TCC_DIR          vendor/tcc)

# TCC
add_library(TCC
    STATIC
      ${TCC_DIR}/libtcc.c
)
target_include_directories(TCC
    PUBLIC
      ${TCC_DIR}
)

# Sources
file(GLOB SD_SOURCE
    CONFIGURE_DEPENDS
    ${SRC_DIR}/*.c
    ${SRC_DIR}/*.h
)

# Executable
add_executable(SourceDiff
    ${SD_SOURCE}
)
target_link_libraries(SourceDiff
    PRIVATE
      TCC
)
