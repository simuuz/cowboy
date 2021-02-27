cmake_minimum_required(VERSION 3.7.0)

project(gb)

set(CMAKE_CXX_STANDARD 17)
set(CORE src/core/)

include_directories(include/)
include_directories(include/core/)

find_package(SDL2 REQUIRED)

add_executable(gb
    src/main.cpp
    ${CORE}cpu.cpp
    ${CORE}mem.cpp)

target_link_libraries(gb SDL2)