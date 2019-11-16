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
set_target_properties(Eigen3::Eigen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/src/eigen_external")