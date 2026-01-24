# Copyright (C) 2026 Raven Computing
# Reckon: A Tool to Count Logical Lines of Code

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(
    reckon
    VERSION 1.0.0
    DESCRIPTION "A Tool to Count Logical Lines of Code"
    HOMEPAGE_URL "https://github.com/raven-computing/reckon"
    LANGUAGES "C"
)

option(RECKON_BUILD_TESTS "Enable/disable tests" OFF)
option(
    RECKON_IGNORE_WARNINGS
    "Specifies whether compiler warnings should let the build \
    process fail (OFF) or whether they should be ignored (ON)"
    OFF
)
option(RECKON_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(RECKON_BUILD_ONLY_LIBS "Build only the Reckon libraries" OFF)
option(RECKON_USE_SANITIZERS "Enable/disable the use of sanitizers" OFF)
option(
    RECKON_BUILD_TEST_COVERAGE
    "Enable/disable test coverage instrumentation"
    OFF
)
option(
    RECKON_SOURCE_ANALYSIS
    "Enable/disable source code static analysis by the compiler"
    OFF
)

set(RECKON_C_STANDARD 17 CACHE STRING "The C standard to use for targets")

if(EXISTS "${PROJECT_SOURCE_DIR}/.cmakeprefixpath")
    message(STATUS "Setting CMake prefix path")
    file(READ ".cmakeprefixpath" _VAR_CMAKE_PREFIX_PATH)
    list(APPEND CMAKE_PREFIX_PATH ${_VAR_CMAKE_PREFIX_PATH})
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/VERSION")
    file(READ "${PROJECT_SOURCE_DIR}/VERSION" RECKON_VERSION)
endif()

include(cmake/BuildUtil.cmake)
include(cmake/TestUtil.cmake)
include(cmake/TestCoverageUtil.cmake)
include(cmake/SanitizerUtil.cmake)

if(NOT DEFINED RECKON_DO_NOT_OVERWRITE_OUTPUT_DIRECTORY)
    set_output_directories()
endif()

include(Dependencies.cmake)

add_subdirectory(src/lib lib)

if(NOT RECKON_BUILD_ONLY_LIBS)
    add_subdirectory(src/scount scount)
endif()

if(RECKON_BUILD_TEST_COVERAGE)
    add_code_coverage_collection_targets()
endif()

if(NOT RECKON_BUILD_ONLY_LIBS)
    include(cmake/ReckonInstallation.cmake)
    set(RECKON_INSTALL_COMPONENT "reckon_scount")
    setup_scount_installation(${RECKON_INSTALL_COMPONENT})
    setup_scount_packaging(${RECKON_INSTALL_COMPONENT})
endif()
