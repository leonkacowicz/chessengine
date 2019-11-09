file(GLOB SRCS *.cpp *.h)
list(REMOVE_ITEM SRCS main.cpp)
add_library(engine_optimizer STATIC ${SRCS})
target_include_directories(engine_optimizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine_optimizer chess)
target_link_libraries(engine_optimizer lib_neuralnet)

add_executable(chessengine_optimizer main.cpp)
target_link_libraries(chessengine_optimizer engine_optimizer)

target_include_directories(engine_optimizer PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ep_boost-prefix/src/ep_boost)
target_link_directories(engine_optimizer PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ep_boost-prefix/src/ep_boost/stage/lib)

