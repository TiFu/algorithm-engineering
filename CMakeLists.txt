cmake_minimum_required(VERSION 2.8)
project(colouring)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ROOT})

add_subdirectory(tests)
