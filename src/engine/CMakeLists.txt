file(GLOB SRCS *.cpp *.h)
add_library(engine STATIC ${SRCS})
target_link_libraries(engine chess)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package (Eigen3 3.3 REQUIRED NO_MODULE)
target_link_libraries(engine Eigen3::Eigen)
target_link_libraries(engine Threads::Threads)