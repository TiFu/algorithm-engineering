cmake_minimum_required(VERSION 2.8)
project(colouring_micro_benchmark)

include(CTest)
enable_testing()

# Disable tests in googlebenchmark
set(BENCHMARK_ENABLE_TESTING OFF)
download_project(
        PROJ googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG master
        UPDATE_DISCONNECTED 1
)
add_subdirectory(${googlebenchmark_SOURCE_DIR} ${googlebenchmark_BINARY_DIR} EXCLUDE_FROM_ALL)
include_directories(SYSTEM "${googlebenchmark_SOURCE_DIR}/include")

add_executable(hca_mb ${INCLUDE} colouring/hca_mb.cpp)
add_executable(xrlf_mb ${INCLUDE} colouring/xrlf_mb.cpp)
target_link_libraries(hca_mb ${CORE_LIBS} benchmark)
target_link_libraries(xrlf_mb ${CORE_LIBS} benchmark)
add_test(colouring_micro_benchmark hca_mb)
