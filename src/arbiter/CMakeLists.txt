file(GLOB SRCS *.cpp *.h)
add_library(arbiter STATIC ${SRCS})
target_include_directories(arbiter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(arbiter chess)

add_executable(chessarbiter arbiter_main.cpp)
target_link_libraries(chessarbiter arbiter)
