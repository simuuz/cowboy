cmake_minimum_required(VERSION 3.7.0)

project(natsukashii)

set(CMAKE_CXX_STANDARD 17)
set(CORE src/core/)

include_directories(include/
	include/core/
	dependencies/mINI/src/mini/)

set(SRC
  src/main.cpp
  src/mainwindow.cpp
  ${CORE}cpu.cpp
  ${CORE}mem.cpp
  ${CORE}bus.cpp
  ${CORE}ppu.cpp)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -Wpedantic -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -Wpedantic -O0 -g")

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(natsukashii
	${SRC}
)

target_link_libraries(natsukashii mingw32 Qt5::Core Qt5::Gui Qt5::Widgets)