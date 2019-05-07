file(GLOB SRCS *.cpp)
add_executable(engine_test ${SRCS})
target_link_libraries(
        engine_test
        engine
        libgtest
        libgmock
)
add_test(NAME engine_test COMMAND engine_test)