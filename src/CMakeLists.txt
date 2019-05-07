add_subdirectory(engine)
add_executable(chessengine main.cpp)
target_link_libraries(chessengine engine)