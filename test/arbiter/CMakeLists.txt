file(GLOB SRCS *.cpp)
add_executable(arbiter_test ${SRCS})
target_link_libraries(
        arbiter_test
        arbiter
        libgtest
        libgmock
)

add_test(NAME arbiter_test COMMAND arbiter_test)
