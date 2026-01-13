# Copyright (C) 2025 Raven Computing
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
# Contains functions providing utilities for documentation resource generation.
# This file may be run in CMake script mode, which will automatically execute
# the configure_doxygen_build() function.
# The minimum CMake version required by this code is v3.22.
#
#==============================================================================

include(FetchContent)

# Performs a configuration step for Doxygen-based documentation builds.
#
# Downloads and configures a Doxygen theme if not already available in the
# build tree. The theme is downloaded into a cache directory to avoid repeated
# downloads. If the environment variable `A_CMAKE_DEPENDENCY_NO_CACHE` is set
# to '1', then the theme will be downloaded directly into the build directory.
#
function(configure_doxygen_build)
    set(THEME_DEPENDENCY "doxygen-theme")
    set(THEME_VERSION "v2.4.1")
    set(CACHE_PATH "$ENV{HOME}/.cache/cmake_deps_src")
    cmake_path(CONVERT "${CACHE_PATH}" TO_CMAKE_PATH_LIST CACHE_PATH NORMALIZE)
    set(THEME_CACHE_PATH "${CACHE_PATH}/${THEME_DEPENDENCY}/${THEME_VERSION}")
    set(THEME_RESOURCE_PATH "${FETCHCONTENT_BASE_DIR}/${THEME_DEPENDENCY}")
    set(
        THEME_RESOURCE_PATH_TMP
        "${FETCHCONTENT_BASE_DIR}/${THEME_DEPENDENCY}-tmp"
    )
    set(THEME_DOWNLOAD OFF)

    if(NOT EXISTS "${THEME_RESOURCE_PATH}")
        if("$ENV{A_CMAKE_DEPENDENCY_NO_CACHE}" STREQUAL "1"
            OR NOT DEFINED ENV{HOME})

            message(STATUS "Doxygen theme cache is disabled")
            set(THEME_DOWNLOAD ON)
            set(THEME_CACHE_PATH "${THEME_RESOURCE_PATH}")
        else()
            if(NOT EXISTS "${THEME_CACHE_PATH}")
                set(THEME_DOWNLOAD ON)
            endif()
        endif()
    endif()

    if(THEME_DOWNLOAD)
        message(STATUS "Downloading Doxygen theme")
        FetchContent_Populate(
            ${THEME_DEPENDENCY}
            GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
            GIT_TAG        ${THEME_VERSION}
            GIT_SHALLOW    ON
            SUBBUILD_DIR   "${THEME_RESOURCE_PATH_TMP}"
            SOURCE_DIR     "${THEME_CACHE_PATH}"
            BINARY_DIR     "${THEME_RESOURCE_PATH_TMP}"
            QUIET
        )
        if(EXISTS "${THEME_RESOURCE_PATH_TMP}")
            file(REMOVE_RECURSE "${THEME_RESOURCE_PATH_TMP}")
        endif()
    endif()

    if(NOT EXISTS "${THEME_RESOURCE_PATH}")
        if(NOT EXISTS "${THEME_CACHE_PATH}")
            message(WARNING "Failed to download Doxygen theme")
            return()
        endif()
        file(
            COPY "${THEME_CACHE_PATH}"
            DESTINATION "${FETCHCONTENT_BASE_DIR}"
        )
        file(
            RENAME "${FETCHCONTENT_BASE_DIR}/${THEME_VERSION}"
            "${THEME_RESOURCE_PATH}"
        )
    endif()

endfunction()

if(CMAKE_SCRIPT_MODE_FILE)
    SET(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/build/_deps")
    configure_doxygen_build()
endif()
