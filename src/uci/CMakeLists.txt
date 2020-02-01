file(GLOB SRCS *.cpp)
add_library(chess_uci STATIC ${SRCS})
target_include_directories(chess_uci PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)