cmake_minimum_required(VERSION 3.10)
project(malloc_hook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(gtest.cmake)

add_subdirectory(malloc_hook)
add_subdirectory(examples)
