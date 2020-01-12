file(GLOB SRCS *.cpp)
add_link_options("-fwhole-program")
add_executable(chess_test ${SRCS})
target_link_libraries(chess_test chess)
target_link_libraries(chess_test libgtest)
target_link_libraries(chess_test libgmock)

add_test(NAME chess_tests COMMAND chess_test)