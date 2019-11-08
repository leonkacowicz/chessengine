add_subdirectory(chess)

add_subdirectory(engine)


add_subdirectory(arbiter)


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
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_subdirectory(engine_optimizer)