cmake_minimum_required(VERSION 3.7.0)

project(natsukashii)

set(CMAKE_C_STANDARD 11)
set(CORE src/core/)

include_directories(include/)
include_directories(include/core/)
include_directories(dependencies/tinyfiledialogs/)
include_directories(dependencies/ini/)

find_package(SDL2 REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -lGL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -lGL -O3")

add_executable(natsukashii
    src/main.c
    ${CORE}natsukashii.c
    ${CORE}cpu.c
    ${CORE}mem.c
    ${CORE}bus.c
    ${CORE}ppu.c
    dependencies/tinyfiledialogs/tinyfiledialogs.c
    dependencies/ini/config.c
)

target_link_libraries(natsukashii SDL2)