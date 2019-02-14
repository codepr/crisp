cmake_minimum_required(VERSION 2.8)

project(crisp)

OPTION(DEBUG "add debug flags" OFF)

if (DEBUG)
    message(STATUS "Configuring build for debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra -std=c11 -O3 -pedantic -ggdb -fsanitize=address -fno-omit-frame-pointer -pg -lm")
else (DEBUG)
    message(STATUS "Configuring build for production")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wunused -Werror -Wextra -std=c11 -O3 -pedantic -lm")
endif (DEBUG)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

file(GLOB SOURCES *.c)

set(AUTHOR "Andrea Giacomo Baldan")
set(LICENSE "BSD2 license")

# Executable
add_executable(crisp ${SOURCES})
