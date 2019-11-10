add_subdirectory(chess)
add_subdirectory(engine)
add_subdirectory(arbiter)
add_subdirectory(engine_optimizer)
add_subdirectory(neuralnet)

include(ExternalProject)
ExternalProject_Add(
        eigen_ext
        URL https://github.com/eigenteam/eigen-git-mirror/archive/master.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/eigen
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON)

ExternalProject_Add (boost_external
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external
        URL https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2
        CONFIGURE_COMMAND ./bootstrap.sh --with-libraries=all
        BUILD_COMMAND ./b2 link=static
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        )

add_library(boost_filesystem IMPORTED STATIC GLOBAL)
add_dependencies(boost_filesystem boost_external)
set_target_properties(boost_filesystem PROPERTIES
        "IMPORTED_LOCATION" ${CMAKE_CURRENT_BINARY_DIR}/external/src/boost_external/stage/lib/libboost_filesystem.a
    )

target_include_directories(boost_filesystem INTERFACE ${CMAKE_BINARY_DIR}/src/external/src/boost_external)
