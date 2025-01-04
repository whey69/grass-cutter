cmake_minimum_required(VERSION 3.10)

project(grass-cutter)
add_executable(grass-cutter main.cpp)
target_compile_options(grass-cutter PRIVATE)
target_link_libraries(grass-cutter ncursesw)
