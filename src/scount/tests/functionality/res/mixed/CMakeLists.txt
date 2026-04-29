# Second CMake sample
cmake_minimum_required(VERSION 3.20)
project(Sample2)
option(ENABLE_TESTS "Enable testing" OFF)

set(CMAKE_C_STANDARD 11)
add_library(mylib STATIC lib.c)
target_include_directories(mylib PUBLIC include/)

install(TARGETS mylib)
