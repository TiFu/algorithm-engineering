cmake_minimum_required(VERSION 2.8)
project(colouring)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Guard against in-source builds and bad build-type strings
include(ConfigSafeGuards)

# Downloads & Compiles external dependencies automatically
include(${CMAKE_MODULE_PATH}/DownloadProject/DownloadProject.cmake)

# OpenMP support
option(USE_OpenMP "Use OpenMP" ON)
if (USE_OpenMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif ()
endif ()

#Common flags and variables
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ROOT}/include)

#Source and include files
set(SRCS src/data_structure/io/graph_io.cpp
        src/util/graph_util.cpp
        src/colouring/graph_colouring.cpp
        src/colouring/crossover/gpx.cpp
        src/colouring/init/greedy_saturation.cpp
        src/colouring/ls/tabu_search.cpp
        src/colouring/hca.cpp)
set(INCLUDES include/util/graph_util.h
        include/data_structure/graph.h
        include/data_structure/io/graph_io.h
        include/colouring/graph_colouring.h
        include/colouring/crossover/gpx.h
        include/colouring/init/greedy_saturation.h
        include/colouring/ls/tabu_search.h
        include/colouring/hca.h)

#Static library packaging
add_library(colouring ${SRCS} ${INCLUDES})
set(CORE_LIBS colouring)

#nested build scripts
add_subdirectory(tests)
add_subdirectory(micro_benchs)
