file(GLOB SRCS *.cpp *.h)
add_library(lib_neuralnet STATIC ${SRCS})
target_include_directories(lib_neuralnet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lib_neuralnet Eigen3::Eigen)