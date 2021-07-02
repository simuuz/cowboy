cmake_minimum_required(VERSION 3.7.0)
project(natsukashii)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE EMU_SRC "src/*.cpp")

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/core/
  ${CMAKE_SOURCE_DIR}/include/core/apu/
  ${CMAKE_SOURCE_DIR}/include/external/
  ${CMAKE_SOURCE_DIR}/include/external/nativefiledialog-extended/
)

find_package(SDL2 REQUIRED)
add_subdirectory(include/external/nativefiledialog-extended/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(natsukashii
  ${IMGUI_SRC}
  ${IMPLOT_SRC}
  ${EMU_SRC}
)

target_link_libraries(${CMAKE_PROJECT_NAME} -static SDL2main SDL2 gcc stdc++ winpthread winmm version Imm32 Setupapi)
target_link_libraries(natsukashii nfd)