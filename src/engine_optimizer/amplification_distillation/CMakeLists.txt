file(GLOB SRCS ampdist.cpp)
add_executable(ampdist ${SRCS})
target_link_libraries(ampdist chess_neuralnet)
target_link_libraries(ampdist engine)
target_link_libraries(ampdist chess)

file(GLOB SRCS parse_log.cpp)
add_executable(parselog ${SRCS})
target_link_libraries(parselog chess_neuralnet)
target_link_libraries(parselog engine)
target_link_libraries(parselog chess)
target_link_libraries(parselog chess_uci)

