file(GLOB MAINCPP main.cpp)
add_executable(engine_diluter ${MAINCPP})
target_link_libraries(engine_diluter chess)
target_link_libraries(engine_diluter boost_include)
target_link_libraries(engine_diluter boost_program_options)

if ("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    add_custom_command(TARGET engine_diluter POST_BUILD COMMAND aws s3 cp ${CMAKE_CURRENT_BINARY_DIR}/engine_diluter s3://leonkacowicz/chess/worker/ || echo "Skipped")
endif()