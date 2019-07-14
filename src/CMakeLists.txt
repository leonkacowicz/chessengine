add_subdirectory(chess)

add_subdirectory(engine)
add_executable(chessengine main.cpp)
target_link_libraries(chessengine chess)
target_link_libraries(chessengine engine)

add_subdirectory(arbiter)
add_executable(chessarbiter arbiter/arbiter_main.cpp)
target_link_libraries(chessarbiter arbiter)

target_compile_options(chessarbiter PUBLIC "-pthread")
target_link_libraries(chessarbiter chess)
target_link_libraries(chessarbiter Threads::Threads)
target_link_libraries(chessarbiter "${CMAKE_THREAD_LIBS_INIT}")