cmake_minimum_required(VERSION 3.7.0)
project(natsukashii)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
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
  dependencies/mINI/src/mini/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wpedantic")

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(natsukashii
  ${SOURCES}
)

target_link_libraries(natsukashii Qt5::Core Qt5::Gui Qt5::Widgets)