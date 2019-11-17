# https://youyue123.github.io/tech/2018/01/29/TDD-for-C++-in-CMake-And-GoogleTest.html
cmake_minimum_required(VERSION 3.5)
project(chessengine)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_link_options(-pthread)
if ("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    set(CMAKE_CXX_FLAGS -O3)
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++17)
endif()

include(dependencies/boost.cmake)
include(dependencies/eigen.cmake)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
