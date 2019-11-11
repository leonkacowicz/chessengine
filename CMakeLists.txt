# https://youyue123.github.io/tech/2018/01/29/TDD-for-C++-in-CMake-And-GoogleTest.html
cmake_minimum_required(VERSION 3.5)
project(chessengine)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_link_options(-pthread)
set(CMAKE_CXX_FLAGS -O3)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++17)
endif()

include(ExternalProject)
ExternalProject_Add(
        eigen_external
        URL https://github.com/eigenteam/eigen-git-mirror/archive/master.zip
        PREFIX ${CMAKE_BINARY_DIR}/external
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON)

add_library(Eigen3::Eigen INTERFACE IMPORTED)
add_dependencies(Eigen3::Eigen eigen_external)
set_target_properties(Eigen3::Eigen PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/src/eigen_external")

ExternalProject_Add (boost_external
        PREFIX ${CMAKE_BINARY_DIR}/external
        URL https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2
        CONFIGURE_COMMAND ./bootstrap.sh --with-libraries=all
        BUILD_COMMAND ./b2 link=static
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        )

add_library(boost_filesystem IMPORTED STATIC GLOBAL)
add_dependencies(boost_filesystem boost_external)
set_target_properties(boost_filesystem PROPERTIES
        "IMPORTED_LOCATION" ${CMAKE_BINARY_DIR}/external/src/boost_external/stage/lib/libboost_filesystem.a
        )

target_include_directories(boost_filesystem INTERFACE ${CMAKE_BINARY_DIR}/external/src/boost_external)


enable_testing()
add_subdirectory(src)
add_subdirectory(test)
