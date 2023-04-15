cmake_minimum_required(VERSION 2.6)
project(monitor)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_subdirectory(src)

option(BUILD_TESTS "Build tests" OFF)
if (BUILD_TESTS)
  add_subdirectory(thirdparty/googletest)
  add_subdirectory(tests)
endif()
