# First CMake sample
cmake_minimum_required(VERSION 3.20)
project(Sample1)
set(SOURCES main.c utils.c)
add_executable(app ${SOURCES})
