cmake_minimum_required(VERSION 3.0)
project(AutoSCP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Boost COMPONENTS filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_subdirectory (src)

add_executable(AutoSCP main.cpp fileutils.cpp fileutils.h)
target_link_libraries (AutoSCP src rsync ssh ${Boost_LIBRARIES})
#LINK_DIRECTORIES()
