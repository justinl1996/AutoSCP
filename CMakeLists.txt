cmake_minimum_required(VERSION 3.0)
project(AutoSCP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
set(CMAKE_CXX_STANDARD 14)

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_subdirectory (src)

add_executable(AutoSCP main.cpp)