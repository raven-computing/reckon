# Sample cmake resource
cmake_minimum_required(VERSION 3.20)
project(Sample)
set(SOURCES main.c)
add_executable(sample ${SOURCES})
set_target_properties(sample PROPERTIES C_STANDARD 11)
target_compile_options(sample PRIVATE -Wall)
install(TARGETS sample)
