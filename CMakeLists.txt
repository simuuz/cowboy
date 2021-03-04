cmake_minimum_required(VERSION 3.7.0)

project(gb)

set(CMAKE_CXX_STANDARD 17)
set(CORE src/core/)
set(IMGUI dependencies/imgui/)

include_directories(include/)
include_directories(include/core/)
include_directories(${IMGUI})
include_directories(${IMGUI}/backends/)
include_directories(dependencies/tinyfiledialogs/)
include_directories(dependencies/mINI/src/mini/)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -lGL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -lGL -O3")

add_executable(gb
    src/main.cpp
    ${CORE}cpu.cpp
    ${CORE}mem.cpp
    ${IMGUI}imgui_demo.cpp
    ${IMGUI}imgui_draw.cpp
    ${IMGUI}imgui_tables.cpp
    ${IMGUI}imgui_widgets.cpp
    ${IMGUI}imgui.cpp
    ${IMGUI}backends/imgui_impl_sdl.cpp
    ${IMGUI}backends/imgui_impl_opengl3.cpp
    dependencies/tinyfiledialogs/tinyfiledialogs.c
)

target_link_libraries(gb SDL2)
target_link_libraries(gb GLEW)