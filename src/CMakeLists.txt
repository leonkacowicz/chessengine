add_subdirectory(chess)

add_subdirectory(engine)
add_executable(chessengine engine/main.cpp)
target_link_libraries(chessengine chess)
target_link_libraries(chessengine engine)

add_subdirectory(arbiter)
add_executable(chessarbiter arbiter/arbiter_main.cpp)
target_link_libraries(chessarbiter arbiter)

target_compile_options(chessarbiter PUBLIC "-pthread")

target_link_libraries(chessarbiter Threads::Threads)
target_link_libraries(chessarbiter "${CMAKE_THREAD_LIBS_INIT}")

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