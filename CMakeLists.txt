cmake_minimum_required(VERSION 3.7.0)

project(natsukashii)

set(CMAKE_CXX_STANDARD 17)
set(CORE src/core/)

include_directories(include/)
include_directories(include/core/)
include_directories(dependencies/tinyfiledialogs/)
include_directories(dependencies/mINI/src/mini/)

find_package(SDL2 REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -lGL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -lGL -O3")

add_executable(natsukashii
    src/main.cpp
    ${CORE}natsukashii.cpp
    ${CORE}cpu.cpp
    ${CORE}mem.cpp
    ${CORE}bus.cpp
    ${CORE}ppu.cpp
    dependencies/tinyfiledialogs/tinyfiledialogs.c
)

target_link_libraries(natsukashii SDL2)