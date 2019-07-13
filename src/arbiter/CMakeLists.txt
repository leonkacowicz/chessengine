set(arbiter_src arbiter.cpp)
add_library(arbiter STATIC ${arbiter_src})
target_include_directories(arbiter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})