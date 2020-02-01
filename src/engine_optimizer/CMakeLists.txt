file(GLOB SRCS *.cpp)
file(GLOB MAINCPP main.cpp)
file(GLOB TRAINSET generate_supervised_training_set.cpp)
file(GLOB TRAINMLP trainmlp.cpp)

list(REMOVE_ITEM SRCS ${MAINCPP})
list(REMOVE_ITEM SRCS ${TRAINSET})
list(REMOVE_ITEM SRCS ${TRAINMLP})

add_library(engine_optimizer STATIC ${SRCS})
target_include_directories(engine_optimizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine_optimizer chess)
target_link_libraries(engine_optimizer lib_neuralnet)
target_link_libraries(engine_optimizer boost_filesystem)

add_executable(chessengine_optimizer ${MAINCPP})
target_link_libraries(chessengine_optimizer engine_optimizer)

add_executable(generate_supervised_training_set ${TRAINSET})
target_link_libraries(generate_supervised_training_set chess)
target_link_libraries(generate_supervised_training_set engine)
target_link_libraries(generate_supervised_training_set lib_neuralnet)
target_link_libraries(generate_supervised_training_set boost_include)

add_executable(trainmlp ${TRAINMLP})
target_link_libraries(trainmlp engine_optimizer)
target_link_libraries(trainmlp boost_include)