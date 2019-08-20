file(GLOB SRCS *.cpp *.h)
add_library(engine_optimizer STATIC ${SRCS})
target_link_libraries(engine_optimizer chess)
target_include_directories(engine_optimizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(engine_optimizer Eigen3::Eigen)
add_executable(chessengine_optimizer main.cpp)
target_link_libraries(chessengine_optimizer engine_optimizer)