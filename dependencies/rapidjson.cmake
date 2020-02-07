include(ExternalProject)
ExternalProject_Add(
        rapidjson_external
        URL https://github.com/Tencent/rapidjson/archive/master.zip
        PREFIX ${CMAKE_BINARY_DIR}/external
        CONFIGURE_COMMAND ""
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON)

add_library(rapidjson INTERFACE IMPORTED)
add_dependencies(rapidjson eigen_external)
#set_target_properties(rapidjson PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/src/rapidjson_external/include")

set(RAPIDJSON_INCLUDE_DIR "${CMAKE_BINARY_DIR}/external/src/rapidjson_external/include")
file(MAKE_DIRECTORY ${RAPIDJSON_INCLUDE_DIR})
target_include_directories(rapidjson INTERFACE ${RAPIDJSON_INCLUDE_DIR})