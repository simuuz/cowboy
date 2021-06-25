cmake_minimum_required(VERSION 3.7.0)
project(natsukashii)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE IMPLOT_SRC "include/external/implot/*.cpp")
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -static-libstdc++ -static-libgcc")

add_executable(natsukashii
  ${IMGUI_SRC}
  ${IMPLOT_SRC}
  ${EMU_SRC}
)

target_link_libraries(natsukashii SDL2)
target_link_libraries(natsukashii nfd)