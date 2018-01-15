cmake_minimum_required(VERSION 3.0)
project(AutoSCP)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

FIND_PACKAGE(Boost COMPONENTS filesystem program_options thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_subdirectory (src)

add_executable(AutoSCP main.cpp ${SRC_FILES})
target_link_libraries (AutoSCP ssh ${Boost_LIBRARIES})
#LINK_DIRECTORIES()
