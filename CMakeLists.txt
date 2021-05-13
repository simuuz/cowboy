cmake_minimum_required(VERSION 3.7.0)

project(natsukashii)

set(CMAKE_CXX_STANDARD 17)
set(CORE src/core/)

include_directories(include/
	include/core/
	dependencies/tinyfiledialogs/
	dependencies/mINI/src/mini/)

find_package(SDL2 REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -O0 -g")

set(SRC
  src/main.cpp
  ${CORE}cpu.cpp
  ${CORE}mem.cpp
  ${CORE}bus.cpp
  ${CORE}ppu.cpp
  dependencies/tinyfiledialogs/tinyfiledialogs.c)

add_executable(natsukashii
	${SRC}
)

target_link_libraries(natsukashii SDL2main SDL2)