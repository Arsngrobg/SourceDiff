cmake_minimum_required(VERSION 4.0)
project(SourceDiff C)

set(CMAKE_C_STANDARD 99                               )
set(SRC_DIR          ${CMAKE_SOURCE_DIR}/src          )
set(TCC_DIR_VENDOR   ${CMAKE_SOURCE_DIR}/vendor/tinycc)
set(TCC_WIN32_DIR    ${TCC_DIR_VENDOR}/win32          )
set(TCC_DIR          ${CMAKE_SOURCE_DIR}/tcc          )

# TCC
file(REMOVE_RECURSE ${TCC_DIR})
execute_process(
    COMMAND           cmd.exe /C "build-tcc.bat > nul"
    WORKING_DIRECTORY ${TCC_WIN32_DIR}
)
file(COPY           ${TCC_WIN32_DIR}          DESTINATION ${CMAKE_SOURCE_DIR})
file(RENAME         ${CMAKE_SOURCE_DIR}/win32             tcc                )
file(REMOVE_RECURSE ${TCC_DIR}/build-tcc.bat)
file(REMOVE_RECURSE ${TCC_DIR}/tcc-win32.txt)
file(REMOVE_RECURSE ${TCC_DIR}/examples)
file(GLOB_RECURSE TCC_SOURCE_POST
    ${TCC_DIR}/lib/*.c
    ${TCC_DIR}/lib/*.S
)
file(REMOVE_RECURSE ${TCC_SOURCE_POST})

# SourceDiff
file(GLOB SD_SOURCE
    CONFIGURE_DEPENDS
    ${SRC_DIR}/*.c
    ${SRC_DIR}/*.h
)
add_executable(SourceDiff
    ${SD_SOURCE}
)
