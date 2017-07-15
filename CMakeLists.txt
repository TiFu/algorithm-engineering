cmake_minimum_required(VERSION 2.8)
project(colouring)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Guard against in-source builds and bad build-type strings
include(ConfigSafeGuards)
# Downloads & Compiles external dependencies automatically
include(${CMAKE_MODULE_PATH}/DownloadProject/DownloadProject.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${ROOT})

add_subdirectory(tests)
