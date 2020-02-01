file(GLOB SRCS *.cpp *.h)
add_library(chess_neuralnet STATIC ${SRCS})
target_include_directories(chess_neuralnet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(chess_neuralnet Eigen3::Eigen)