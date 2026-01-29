cmake_minimum_required(VERSION 3.20)
project(nNewton VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#folder organization
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Build options
option(BUILD_TESTBED "Build the testbed application" ON)
option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# config types
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING 
        "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# subdirectories for external-dependencies
add_subdirectory(external)

#nNewton library
add_subdirectory(src)

#testbed
if(BUILD_TESTBED)
    add_subdirectory(SandBox)
endif()


# Installation targets
include(GNUInstallDirs)
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
