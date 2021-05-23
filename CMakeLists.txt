cmake_minimum_required(VERSION 3.7.0)
project(natsukashii)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)

set(ImGui
  include/external/imgui/imconfig.h
  include/external/imgui/imgui.cpp
  include/external/imgui/imgui.h
  include/external/imgui/imgui_demo.cpp
  include/external/imgui/imgui_draw.cpp
  include/external/imgui/imgui_internal.h
  include/external/imgui/imgui_widgets.cpp
  include/external/imgui/imstb_rectpack.h
  include/external/imgui/imstb_textedit.h
  include/external/imgui/imstb_truetype.h
  include/external/imgui/imgui_tables.cpp
  include/external/imgui/backends/imgui_impl_opengl3.cpp
  include/external/imgui/backends/imgui_impl_opengl3.h
  include/external/imgui/backends/imgui_impl_glfw.cpp
  include/external/imgui/backends/imgui_impl_glfw.h)

set(MBCS
  src/core/mbc/nombc.cpp
  src/core/mbc/mbc1.cpp
  src/core/mbc/mbc2.cpp
  src/core/mbc/mbc3.cpp
  src/core/mbc/mbc5.cpp)

set(SOURCES
  ${ImGui}
  ${MBCS}
  src/core/mem.cpp
  src/core/cpu.cpp
  src/core/bus.cpp
  src/core/ppu.cpp
  src/core.cpp
  src/main.cpp
  src/mainwindow.cpp)

include_directories(
  include/
  include/core/
  include/external/
  include/external/imgui/
  include/external/imgui/backends/
  include/external/nativefiledialog-extended/src/include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_subdirectory(include/external/nativefiledialog-extended/)

add_executable(natsukashii
  ${SOURCES}
)

target_link_libraries(natsukashii PRIVATE GLEW)
target_link_libraries(natsukashii PRIVATE OpenGL ${CMAKE_DL_LIBS})
target_link_libraries(natsukashii PRIVATE glfw)
target_link_libraries(natsukashii PRIVATE nfd)