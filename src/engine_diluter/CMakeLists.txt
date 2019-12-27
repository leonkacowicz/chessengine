file(GLOB MAINCPP main.cpp)
add_executable(engine_diluter ${MAINCPP})
target_link_libraries(engine_diluter chess)
target_link_libraries(engine_diluter boost_filesystem)