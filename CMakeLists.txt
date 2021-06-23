cmake_minimum_required(VERSION 3.7.0)
project(natsukashii)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE IMPLOT_SRC "include/external/implot/*.cpp")
file(GLOB_RECURSE EMU_SRC "src/*.cpp")

set(IMGUI_SRC
  ${CMAKE_SOURCE_DIR}/include/external/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/include/external/imgui/imgui_demo.cpp
  ${CMAKE_SOURCE_DIR}/include/external/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/include/external/imgui/imgui_tables.cpp
  ${CMAKE_SOURCE_DIR}/include/external/imgui/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/include/external/imgui/backends/imgui_impl_opengl3.cpp
  ${CMAKE_SOURCE_DIR}/include/external/imgui/backends/imgui_impl_glfw.cpp)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/core/
  ${CMAKE_SOURCE_DIR}/include/core/apu/
  ${CMAKE_SOURCE_DIR}/include/external/
  ${CMAKE_SOURCE_DIR}/include/external/imgui/
  ${CMAKE_SOURCE_DIR}/include/external/imgui/backends/
  ${CMAKE_SOURCE_DIR}/include/external/implot/
  ${CMAKE_SOURCE_DIR}/include/external/nativefiledialog-extended/
  ${CMAKE_SOURCE_DIR}/include/external/miniaudio/
)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
add_subdirectory(include/external/nativefiledialog-extended/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -static-libstdc++ -static-libgcc")

add_executable(natsukashii
  ${IMGUI_SRC}
  ${IMPLOT_SRC}
  ${EMU_SRC}
)

target_link_libraries(natsukashii SDL2)
target_link_libraries(natsukashii OpenGL::GL)
target_link_libraries(natsukashii glfw)
target_link_libraries(natsukashii GLEW::glew)
target_link_libraries(natsukashii nfd)