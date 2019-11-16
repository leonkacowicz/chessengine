include(ExternalProject)
ExternalProject_Add(boost_external
        PREFIX ${CMAKE_BINARY_DIR}/external
        URL https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2
        CONFIGURE_COMMAND ./bootstrap.sh --with-libraries=all
        BUILD_COMMAND ./b2 link=static
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        )

foreach(lib "filesystem" "program_options")
    set(libname "boost_${lib}")
    message("Adding ${libname}")
    add_library(${libname} IMPORTED STATIC GLOBAL)
    add_dependencies(${libname} boost_external)
    set_target_properties(${libname} PROPERTIES "IMPORTED_LOCATION" ${CMAKE_BINARY_DIR}/external/src/boost_external/stage/lib/lib${libname}.a)
    target_include_directories(${libname} INTERFACE ${CMAKE_BINARY_DIR}/external/src/boost_external)
endforeach()
