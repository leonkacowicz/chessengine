file(GLOB MAINCPP main.cpp)
add_executable(random_engine ${MAINCPP})
target_link_libraries(random_engine chess)
