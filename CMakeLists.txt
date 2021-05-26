cmake_minimum_required(VERSION 3.7.0)
project(natsukashii)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE IMPLOT_SRC "include/external/implot/*.cpp")
set(IMGUI_SRC
    include/external/imgui/imgui.cpp
    include/external/imgui/imgui_demo.cpp
    include/external/imgui/imgui_draw.cpp
    include/external/imgui/imgui_tables.cpp
    include/external/imgui/imgui_widgets.cpp
    include/external/imgui/backends/imgui_impl_opengl3.cpp
    include/external/imgui/backends/imgui_impl_glfw.cpp)
file(GLOB_RECURSE EMU_SRC "src/*.cpp")

include_directories(
  include/
  include/core/
  include/external/
  include/external/imgui/
  include/external/imgui/backends/
  include/external/implot/
  include/external/nativefiledialog-extended/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_subdirectory(include/external/nativefiledialog-extended/)

add_executable(natsukashii
  ${IMPLOT_SRC}
  ${IMGUI_SRC}
  ${EMU_SRC}
)

target_link_libraries(natsukashii PRIVATE GLEW)
target_link_libraries(natsukashii PRIVATE OpenGL ${CMAKE_DL_LIBS})
target_link_libraries(natsukashii PRIVATE glfw)
target_link_libraries(natsukashii PRIVATE nfd)