cmake_minimum_required(VERSION 2.8)
project(colouring)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Guard against in-source builds and bad build-type strings
include(ConfigSafeGuards)
# Downloads & Compiles external dependencies automatically
include(${CMAKE_MODULE_PATH}/DownloadProject/DownloadProject.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${ROOT}/include)

set(SRCS src/data_structure/io/graph_io.cpp
        src/util/graph_util.cpp
        src/colouring/graph_colouring.cpp
        src/colouring/greedy_crossover.cpp)

set(INCLUDES include/util/graph_util.h
        include/data_structure/graph.h
        include/data_structure/io/graph_io.h
        include/colouring/graph_colouring.h
        include/colouring/greedy_crossover.h)

add_library(colouring ${SRCS} ${INCLUDES})

set(CORE_LIBS colouring)

add_subdirectory(tests)
