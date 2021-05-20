cmake_minimum_required(VERSION 3.7.0)
project(natsukashii)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MBCS
  src/core/mbc/nombc.cpp
  src/core/mbc/mbc1.cpp
  src/core/mbc/mbc2.cpp
  src/core/mbc/mbc3.cpp
  src/core/mbc/mbc5.cpp)

set(SOURCES
  ${MBCS}
  ${NFD}
  src/core/mem.cpp
  src/core/cpu.cpp
  src/core/bus.cpp
  src/core/ppu.cpp
  src/core.cpp
  src/main.cpp
  src/renderer.cpp
  src/mainwindow.cpp)

include_directories(
  include/
  include/core/
  include/external/
  include/external/imgui/
  include/external/nativefiledialog-extended/src/include/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")

find_package(ImGui-SFML REQUIRED)

add_subdirectory(include/external/nativefiledialog-extended/)

add_executable(natsukashii
  ${SOURCES}
)

target_link_libraries(natsukashii PRIVATE nfd)
target_link_libraries(natsukashii PRIVATE ImGui-SFML::ImGui-SFML)
