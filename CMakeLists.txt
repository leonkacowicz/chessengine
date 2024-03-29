# https://youyue123.github.io/tech/2018/01/29/TDD-for-C++-in-CMake-And-GoogleTest.html
cmake_minimum_required(VERSION 3.5)
project(chessengine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_link_options(-pthread)
if ("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -msse -msse3 -mpopcnt -DUSE_POPCNT -msse4 -mbmi2 -fopenmp")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++2a)
endif()

include(dependencies/boost.cmake)
include(dependencies/eigen.cmake)
include(dependencies/rapidjson.cmake)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
