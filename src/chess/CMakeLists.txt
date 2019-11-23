file(GLOB SRCS *.cpp)
add_library(chess STATIC ${SRCS})
target_include_directories(chess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})