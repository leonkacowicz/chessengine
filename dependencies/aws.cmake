include(ExternalProject)
list(APPEND cmake_args "-DBUILD_ONLY=sqs" "-DCMAKE_BUILD_TYPE=Release" "-DBUILD_SHARED_LIBS=false"
        "-DENABLE_TESTING:BOOL=OFF" "-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON" "-DMINIMIZE_SIZE:BOOL=ON"
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/src/aws_external/install)

ExternalProject_Add(
        aws_external
        URL https://github.com/aws/aws-sdk-cpp/archive/master.zip
        PREFIX ${CMAKE_BINARY_DIR}/external
        CMAKE_ARGS ${cmake_args}
        #CONFIGURE_COMMAND "/opt/jetbrains/clion-2018.3.2/bin/cmake/linux/bin/cmake"
        #BUILD_IN_SOURCE true
        INSTALL_DIR ${CMAKE_BINARY_DIR}/external/src/aws_external/install
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON)



#add_library(Aws IMPORTED STATIC GLOBAL)
#add_dependencies(Aws aws_external)
#set_target_properties(Aws PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/external/src/aws_external/install/lib/libaws-cpp-sdk-core.a")
#set_target_properties(Aws PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/src/aws_external/install/include")
#
#add_library(Aws::Sqs IMPORTED STATIC GLOBAL)
#add_dependencies(Aws::Sqs Aws)
#set_target_properties(Aws::Sqs PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/external/src/aws_external/install/lib/libaws-cpp-sdk-sqs.a")
#set_target_properties(Aws::Sqs PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/src/aws_external/install/include")

set(aws-sdk-cpp_DIR "${CMAKE_BINARY_DIR}/external/src/aws_external/install/")
set(AWSSDK_DIR "${CMAKE_BINARY_DIR}/external/src/aws_external/install/lib/cmake/AWSSDK")
find_package(AWSSDK REQUIRED COMPONENTS sqs)