cmake_minimum_required(VERSION 3.2)
project(colouring)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Guard against in-source builds and bad build-type strings
include(ConfigSafeGuards)

# Downloads & Compiles external dependencies automatically
include(${CMAKE_MODULE_PATH}/DownloadProject/DownloadProject.cmake)

#Boost
set(BOOST_REQUESTED_VERSION 1.61.0)
find_package(Boost)

#Common flags and variables
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ROOT})
include_directories(${ROOT}/framework)

#Source and include files

file(GLOB_RECURSE SRCS
        "data_structure/*.cpp"
        "framework/*.cpp")

file(GLOB_RECURSE INCLUDES
        "data_structure/*.h"
        "framework/*.h")

#Static library packaging
add_library(colouring ${SRCS} ${INCLUDES})
set(CORE_LIBS colouring)

#nested build scripts
add_subdirectory(tests)
add_subdirectory(micro_benchs)
