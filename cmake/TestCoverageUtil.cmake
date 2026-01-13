# Copyright (C) 2026 Raven Computing
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#==============================================================================
#
# Defines functions for creating targets to support code test coverage and
# adding compiler options to targets in CMake-based projects to add support
# for code test coverage instrumentation.
# The minimum CMake version required by this code is v3.22.
#
#==============================================================================

# Adds code test coverage support to a given CMake target.
#
# Appends the appropriate compiler flags to enable code test
# coverage instrumentation for the specified target.
# Code coverage should only be enabled for Debug builds.
#
# Arguments:
#
#   target_name:
#       The name of the target to which code coverage support will be added.
#       This argument is mandatory.
#
# Example:
#   add_code_coverage(mytarget)
#
function(add_code_coverage target_name)
    if(WIN32 AND CMAKE_GENERATOR MATCHES "^Visual Studio")
        message(
            WARNING
            "Building code with test coverage instrumentation "
            "is not available on Windows with the Visual Studio generator. "
            "If you want to enable the use of code coverage metrics "
            "on Windows, please build with GCC via MSYS2/MinGW."
        )
        return()
    endif()
    if((DEFINED CMAKE_C_COMPILER_ID AND NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
        OR (DEFINED CMAKE_CXX_COMPILER_ID
            AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
        if(MSYS)
            message(
                WARNING
                "When building with code test coverage support on Windows "
                "with MSYS2/MinGW, GCC must be used as the compiler."
            )
        else()
            message(
                WARNING
                "When building with code test coverage support, "
                "only GCOV via GCC is currently supported."
            )
        endif()
        return()
    endif()

    string(TOUPPER ${CMAKE_BUILD_TYPE} PROJECT_BUILD_TYPE)
    message(
        STATUS "Adding code coverage instrumentation to target ${target_name}"
    )
    if(NOT ${PROJECT_BUILD_TYPE} STREQUAL "DEBUG")
        message(
            WARNING
            "Code test coverage measurements should only be performed "
            "with a non-optimized Debug build"
        )
    endif()

    target_compile_options(
        ${target_name}
        PRIVATE
        -ftest-coverage -fprofile-arcs -fno-default-inline
        $<$<COMPILE_LANGUAGE:CXX>:-fno-elide-constructors>
    )
    target_link_libraries(
        ${target_name}
        PRIVATE
        gcov
    )

endfunction()

# Creates CMake targets to clean, collect and merge coverage data with lcov.
#
# Adds the following targets to the current CMake project:
#
#   - clean_coverage_data: Cleans up old coverage data files
#   - collect_coverage_data: Collects & merges coverage data into a single file
#
# The targets will only be added if the build is performed with GCC and
# the lcov tool is available on the system.
#
# Usage:
#   add_code_coverage_collection_targets()
#
function(add_code_coverage_collection_targets)
    set(output_dir "${CMAKE_BINARY_DIR}/cov")
    set(output_file "${output_dir}/merged.info")
    set(include_glob "${CMAKE_SOURCE_DIR}/src/*")
    set(exclude_glob "${CMAKE_SOURCE_DIR}/src/**/debug.c")
    set(cov_clean_script "${CMAKE_BINARY_DIR}/CMakeFiles/clean-cov-gcda.cmake")

    message(
        CHECK_START
        "Checking if test coverage collection targets can be added"
    )

    if(DEFINED CMAKE_C_COMPILER_ID AND NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
        message(CHECK_FAIL "Not available")
        message(
            WARNING
            "When building with code test coverage support, "
            "only GCOV via GCC is currently supported."
        )
        return()
    endif()

    find_program(LCOV_EXEC lcov)
    if(NOT LCOV_EXEC)
        message(CHECK_FAIL "Not available")
        message(
            WARNING
            "Could not find the 'lcov' command. "
            "Please install lcov to collect test coverage data."
        )
        return()
    endif()

    message(CHECK_PASS "Available")
    file(
        COPY_FILE
        "${CMAKE_SOURCE_DIR}/cmake/TestCoverageCleanupUtil.cmake"
        "${cov_clean_script}"
    )
    add_custom_target(
        clean_coverage_data
        COMMAND ${CMAKE_COMMAND} -P "${cov_clean_script}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Cleaning up old coverage data (*.gcda files)"
        VERBATIM
    )
    add_custom_target(
        collect_coverage_data
        COMMAND ${CMAKE_COMMAND} -E make_directory "${output_dir}"
        COMMAND ${LCOV_EXEC} --quiet
                             --directory "${CMAKE_BINARY_DIR}"
                             --capture
                             --include "${include_glob}"
                             --exclude "${exclude_glob}"
                             --output-file "${output_file}"
                             --rc geninfo_unexecuted_blocks=1
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Collecting coverage data into ${output_file}"
        VERBATIM
    )

endfunction()
