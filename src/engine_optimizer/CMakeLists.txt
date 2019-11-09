file(GLOB SRCS *.cpp *.h)
add_library(engine_optimizer STATIC ${SRCS})
target_include_directories(engine_optimizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine_optimizer chess)
target_link_libraries(engine_optimizer Eigen3::Eigen)
add_executable(chessengine_optimizer main.cpp)
target_link_libraries(chessengine_optimizer engine_optimizer)

target_include_directories(engine_optimizer PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ep_boost-prefix/src/ep_boost)
target_link_directories(engine_optimizer PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ep_boost-prefix/src/ep_boost/stage/lib)

include(ExternalProject)
ExternalProject_Add (ep_boost
        URL https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2
        CONFIGURE_COMMAND ./bootstrap.sh --with-libraries=all
        BUILD_COMMAND ./b2 link=static
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        )