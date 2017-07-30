cmake_minimum_required(VERSION 2.8)
project(colouring_test)

include(CTest)
enable_testing()

#set(gtest_disable_pthreads on) #needed in MinGW
download_project(
        PROJ                googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             master
        UPDATE_DISCONNECTED 1
)
include_directories(SYSTEM "${googletest_SOURCE_DIR}/googletest/include")
include_directories(SYSTEM "${googletest_SOURCE_DIR}/googlemock/include")
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

file(GLOB_RECURSE TEST_SRCS
        "*.cpp")

add_executable(colouring_test ${INCLUDE} ${TEST_SRCS})
target_link_libraries(colouring_test ${CORE_LIBS} gtest gmock)
add_test(colouring_test colouring_test)