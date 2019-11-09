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

ExternalProject_Add (ep_boost
        URL https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2
        CONFIGURE_COMMAND ./bootstrap.sh --with-libraries=all
        BUILD_COMMAND ./b2 link=static
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        )