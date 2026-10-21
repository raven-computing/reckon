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
# Contains functions providing utilities for program performance profiling.
# The minimum CMake version required by this code is v3.22.
#
#==============================================================================

include(FetchContent)

# Adds performance profiling support to a given CMake target.
#
# Appends the appropriate compiler and linker flags to enable performance
# profiling for the specified target. Currently, GNU/Linux is the only
# supported platform.
#
# Arguments:
#
#   target_name:
#       The name of the target to which profiling support will be added.
#       This argument is mandatory.
#
# Example:
#   add_performance_profiling(mytarget)
#
function(add_performance_profiling target_name)

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message(
            WARNING
            "Performance profiling is not supported on ${CMAKE_SYSTEM_NAME}"
        )
        return()
    endif()

    target_compile_options(
        ${target_name}
        PUBLIC
        "-fno-omit-frame-pointer"
    )

    message(
        STATUS
        "Performance profiling support enabled for target ${target_name}"
    )

endfunction()

# Performs the configuration step for the performance profiling flamegraph
# generation utility.
#
# Downloads and configures the flamegraph tool if not already available in the
# build tree. The tool is downloaded into a cache directory to avoid repeated
# downloads. If the environment variable `A_CMAKE_DEPENDENCY_NO_CACHE` is set
# to '1', then the tool will be downloaded directly into the build directory.
#
function(configure_performance_profiling_dependency)
    set(FLAMEGRAPH_DEPENDENCY "perf-flamegraph")
    set(FLAMEGRAPH_VERSION "41fee1f99f9276008b7cd112fca19dc3ea84ac32")
    SET(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/build/_deps")
    set(CACHE_PATH "$ENV{HOME}/.cache/cmake_deps_src")
    cmake_path(CONVERT "${CACHE_PATH}" TO_CMAKE_PATH_LIST CACHE_PATH NORMALIZE)
    set(
        FLAMEGRAPH_CACHE_PATH
        "${CACHE_PATH}/${FLAMEGRAPH_DEPENDENCY}/${FLAMEGRAPH_VERSION}"
    )
    set(
        FLAMEGRAPH_RESOURCE_PATH
        "${FETCHCONTENT_BASE_DIR}/${FLAMEGRAPH_DEPENDENCY}"
    )
    set(
        FLAMEGRAPH_RESOURCE_PATH_TMP
        "${FETCHCONTENT_BASE_DIR}/${FLAMEGRAPH_DEPENDENCY}-tmp"
    )
    set(FLAMEGRAPH_DOWNLOAD OFF)

    if(NOT EXISTS "${FLAMEGRAPH_RESOURCE_PATH}")
        if("$ENV{A_CMAKE_DEPENDENCY_NO_CACHE}" STREQUAL "1"
            OR NOT DEFINED ENV{HOME})

            message(
                STATUS
                "Performance profiling flamegraph cache is disabled"
            )
            set(FLAMEGRAPH_DOWNLOAD ON)
            set(FLAMEGRAPH_CACHE_PATH "${FLAMEGRAPH_RESOURCE_PATH}")
        else()
            if(NOT EXISTS "${FLAMEGRAPH_CACHE_PATH}")
                set(FLAMEGRAPH_DOWNLOAD ON)
            endif()
        endif()
    endif()

    if(FLAMEGRAPH_DOWNLOAD)
        message(STATUS "Downloading performance profiling flamegraph utility")
        FetchContent_Populate(
            ${FLAMEGRAPH_DEPENDENCY}
            GIT_REPOSITORY https://github.com/brendangregg/FlameGraph.git
            GIT_TAG        ${FLAMEGRAPH_VERSION}
            GIT_SHALLOW    ON
            SUBBUILD_DIR   "${FLAMEGRAPH_RESOURCE_PATH_TMP}"
            SOURCE_DIR     "${FLAMEGRAPH_CACHE_PATH}"
            BINARY_DIR     "${FLAMEGRAPH_RESOURCE_PATH_TMP}"
            QUIET
        )
        if(EXISTS "${FLAMEGRAPH_RESOURCE_PATH_TMP}")
            file(REMOVE_RECURSE "${FLAMEGRAPH_RESOURCE_PATH_TMP}")
        endif()
    endif()

    if(NOT EXISTS "${FLAMEGRAPH_RESOURCE_PATH}")
        if(NOT EXISTS "${FLAMEGRAPH_CACHE_PATH}")
            message(
                WARNING
                "Failed to download performance profiling flamegraph utility"
            )
            return()
        endif()
        file(
            COPY "${FLAMEGRAPH_CACHE_PATH}"
            DESTINATION "${FETCHCONTENT_BASE_DIR}"
        )
        file(
            RENAME "${FETCHCONTENT_BASE_DIR}/${FLAMEGRAPH_VERSION}"
            "${FLAMEGRAPH_RESOURCE_PATH}"
        )
    endif()

endfunction()
