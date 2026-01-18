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
# This file contains a function for specifying dependencies in CMake-based
# projects. It is a thin wrapper around the FetchContent machinery provided
# by CMake. The minimum CMake version required by this code is v3.22.
#
#==============================================================================

include(FetchContent)

# This function can be used by CMake projects to declare dependencies as
# either Git repositories or source archive files. It is a thin wrapper
# around the FetchContent machinery provided by CMake. By using this function,
# a dependency becomes available to the underlying project in a
# declarative way. There are four steps involved in processing a dependency:
# First, the specified dependency is passed to the CMake FetchContent_Declare()
# function. The caller does not need to take care of the different arguments
# as used by FetchContent_Declare(), as they are handled entirely by
# this function. Secondly, an additional configuration file is potentially
# included by this function. The purpose of such a file is to configure the
# dependency to be made available to the underlying project. The configuration
# file can contain arbitrary CMake code. Thirdly, the dependency is made
# available by means of the CMake FetchContent_MakeAvailable() function.
# This steps makes the declared targets of the dependency available to
# the underlying project. Lastly, an additional build file is potentially
# included by this function. This is similar to the configuration step, but
# it allows a project to potentially manipulate the dependency target and its
# build directly.
#
# The configuration and build files for a dependency are ordinary CMake files,
# i.e. it must contain syntactically valid CMake code. Each dependency
# configuration and build file included by this function if found.
# The configuration file can be used to set variables or do any other kind of
# processing in order to properly configure the dependency to be used by the
# underlying project. If no concrete config file is specified by
# the DEPENDENCY_CONFIG_FILE argument, then this function will look for a
# dependency config file in the "cmake" directory relative
# to CMAKE_CURRENT_SOURCE_DIR. The corresponding config file must be
# named "Config${DEPENDENCY_NAME}.cmake", where ${DEPENDENCY_NAME} is the
# value of the mandatory dependency name argument. Please note that the
# first character of the otherwise lowercase dependency name is always
# capitalised, resulting in a Pascal-case-style file name. For example, if a
# concrete dependency is named "mydep", then the conventional config file
# has to be named "ConfigMydep.cmake". The inclusion of any dependency config
# file can be disabled by using the DEPENDENCY_NO_CONFIG option.
# The same convention applies to the build file, with a varying file prefix
# of "Build*" instead of "Config*". For example, a conventional build file
# might be named "BuildMydep.cmake".
#
# Every dependency has a scope. A dependency is only used if its scope
# is active. The following scopes are available: "ANY", "RELEASE", "TEST"
# and "DEBUG". The "ANY" scope is the default scope and will cause the
# dependency to be used in any case. The "RELEASE" scope is active if the
# project is configured to produce a release build. This is true when the
# standard CMake variable CMAKE_BUILD_TYPE is equal to "Release". Please note
# that other release build types like "RelWithDebInfo" or "MinSizeRel" are not
# considered by this scope. The "TEST" scope can be used for dependencies only
# needed to run the project tests. This scope is active if the *_BUILD_TESTS
# variable is true, where the "*" represents the project name as given to the
# CMake project() function, in all upper case. The project name is taken from
# the standard PROJECT_NAME CMake variable. The "DEBUG" scope is active if the
# project is configured to produce a debug build. This is true when the
# standard CMake variable CMAKE_BUILD_TYPE is equal to "Debug".
#
# By default, this function attempts to cache all dependency sources outside of
# the build tree. This is so that subsequent clean builds can reuse the sources
# without having to refetch them over a network connection. When a project has
# numerous and/or large dependencies, this can significantly speed up its
# build configure phase. Additionally, a project can potentially be rebuilt
# without requiring a network connection. In order for a dependency source to
# be cached, the DEPENDENCY_VERSION argument must be specified. All sources are
# cached under the underlying user's home directory, which requires that
# the HOME environment variable is set. Sources are cached under
# the '${HOME}/.cache/cmake_deps_src' directory, although this should be
# considered an implementation detail and may change in the future without
# further notice. Further, if a cache directory inside the project source tree
# already exists, under '${CMAKE_CURRENT_SOURCE_DIR}/.cache/cmake_deps_src',
# the project-specific cache will be used instead of the one in the user's home
# directory. The caching of the sources for a specific dependency can be
# disabled by using the DEPENDENCY_NO_CACHE option argument. To generally
# disable the caching of all dependency sources, set the environment variable
# A_CMAKE_DEPENDENCY_NO_CACHE to the value '1'.
#
# Arguments:
#
#   [oneValueArgs]
#
#   DEPENDENCY_NAME:
#       The name of the dependency, as a lowercase string. Must be unique
#       throughout the entire project. This argument is mandatory.
#
#   DEPENDENCY_RESOURCE:
#       The source code resource of the dependency.
#       This can be specified in two ways. It can either point to a Git
#       repository or a remote/local archive file containg the source.
#       In the case of a Git repository, it can be specified as the entire URL
#       pointing to the Git repository (long form), or simply specified as the
#       organisation and repository name separated with a slash ('/')
#       delimiter (short form). The short form, e.g. 'myorga/myrepo', will be
#       autocompleted to a full GitHub repository URL.
#       In the case of a file, it must represent a valid archive file
#       in either ZIP or TAR format. An archive file can be specified as
#       a URL, in which case the file will be fetched from that URL.
#       Supported protocols for archive file URLs are "http://", "https://"
#       and "file://" (for files within the local filesystem).
#       This argument is mandatory.
#
#   DEPENDENCY_VERSION:
#       The version specifier of the dependency.
#       If the dependency resource is a Git repository, then this argument
#       must be specified. It can then either represent a Git branch,
#       a Git tag or a Git commit hash. If the dependency resource is an
#       archive file, then this argument is optional and has purely
#       declarative purposes without any real effects.
#
#   DEPENDENCY_SCOPE:
#       The scope of the dependency. Supported scopes are "ANY", "RELEASE",
#       "TEST" and "DEBUG". This determines whether the dependency is used
#       or not, depending on whether the specified scope is active. This
#       argument is optional and can be omitted.
#       The default scope is "ANY".
#
#   DEPENDENCY_FILE_HASH:
#       The SHA256 hash value of the dependency resource archive file.
#       This argument must only be used when the dependency resource represents
#       an archive file. The hash must be specified as a hex string. It is used
#       to verify the integrity of the fetched file. It is a mandatory argument
#       for archive file dependency resources.
#
#   DEPENDENCY_CONFIG_FILE:
#       The relative path to the file containing CMake instructions to be used
#       to configure the dependency. This argument can optionally be used to
#       specify a file which deviates from the convention.
#
#   DEPENDENCY_BUILD_FILE:
#       The relative path to the file containing CMake instructions to be used
#       to build the dependency. This argument can optionally be used to
#       specify a file which deviates from the convention.
#
#   [multiValueArgs]
#
#   DEPENDENCY_LINK_TARGETS:
#       A list of target names that should be registered as link targets when
#       consuming the dependency. This argument is optional. When specified,
#       the provided names represent CMake targets that are made available to
#       the underlying project by the dependency. These target names are then
#       appended to the global variable ${PROJECT}_DEPENDENCIES_LINK_TARGETS
#       where ${PROJECT} is the name in all caps of the underlying project as
#       given to the CMake project() function. A dependency with "TEST" scope
#       is never added to that list.
#
#   [options]
#
#   DEPENDENCY_QUIET:
#       Suppresses all output with regard to setting up the dependecy,
#       except fatal errors.
#
#   DEPENDENCY_VERBOSE:
#       Shows verbose output with regard to setting up the dependecy.
#       This option does not usually need to be used but can be helpful
#       to troubleshoot a failing dependency.
#
#   DEPENDENCY_NO_CONFIG:
#       Disables the inclusion of any dependency config files.
#
#   DEPENDENCY_NO_CACHE:
#       Disables the source cache. This has the effect that if the dependency
#       sources are not available in the build tree, they are fetched, stored
#       within the build tree (as opposed to the user-specific cache) and
#       reused in subsequent builds. When the project's build tree is cleaned,
#       the sources of the dependency are also removed and need to be refetched
#       in subsequent builds. When this option is specified, the user-specific
#       source cache is never considered.
#
# Example usage:
#
# dependency(
#     DEPENDENCY_NAME         unity
#     DEPENDENCY_RESOURCE     https://github.com/ThrowTheSwitch/Unity.git
#     DEPENDENCY_VERSION      v2.5.2
#     DEPENDENCY_SCOPE        TEST
# )
#
function(dependency)

    set(options
        DEPENDENCY_QUIET
        DEPENDENCY_NO_CONFIG
        DEPENDENCY_VERBOSE
        DEPENDENCY_NO_CACHE
    )
    set(oneValueArgs
        DEPENDENCY_NAME
        DEPENDENCY_RESOURCE
        DEPENDENCY_VERSION
        DEPENDENCY_SCOPE
        DEPENDENCY_FILE_HASH
        DEPENDENCY_CONFIG_FILE
        DEPENDENCY_BUILD_FILE
    )
    set(multiValueArgs
        DEPENDENCY_LINK_TARGETS
    )

    cmake_parse_arguments(
        DEP_ARGS
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # The default URL to use for dependencies declared
    # with the DEPENDENCY_RESOURCE short form
    set(DEPENDENCY_BASE_URL_DEFAULT "https://github.com")

    # Check debug scope
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(SCOPE_DEBUG_ACTIVE 1)
    else()
        set(SCOPE_DEBUG_ACTIVE 0)
    endif()

    string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
    string(TOUPPER ${DEP_ARGS_DEPENDENCY_NAME} DEP_ARGS_DEPENDENCY_NAME_UPPER)

    # Check test scope
    if(${${PROJECT_NAME_UPPER}_BUILD_TESTS})
        set(SCOPE_TEST_ACTIVE 1)
    else()
        set(SCOPE_TEST_ACTIVE 0)
    endif()

    # Check release scope
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(SCOPE_RELEASE_ACTIVE 1)
    else()
        set(SCOPE_RELEASE_ACTIVE 0)
    endif()

    if(NOT DEP_ARGS_DEPENDENCY_SCOPE)
        set(DEPENDENCY_SCOPE ANY)
    else()
       set(DEPENDENCY_SCOPE ${DEP_ARGS_DEPENDENCY_SCOPE})
    endif()

    set(DEP_ALLOWED_SCOPES ANY RELEASE TEST DEBUG)
    set(DEP_TRACKED_SCOPES ANY RELEASE DEBUG)

    if(NOT "${DEPENDENCY_SCOPE}" IN_LIST DEP_ALLOWED_SCOPES)
        message(
            FATAL_ERROR
            "Invalid scope argument for "
            "dependency ${DEP_ARGS_DEPENDENCY_NAME}: "
            "Expected any of ${DEP_ALLOWED_SCOPES} but "
            "found ${DEPENDENCY_SCOPE}"
        )
    endif()

    # Check if specified scope is active
    if("${DEPENDENCY_SCOPE}" STREQUAL "DEBUG")
        if(NOT ${SCOPE_DEBUG_ACTIVE})
            return()
        endif()
    endif()

    if("${DEPENDENCY_SCOPE}" STREQUAL "TEST")
        if(NOT ${SCOPE_TEST_ACTIVE})
            return()
        endif()
    endif()

    if("${DEPENDENCY_SCOPE}" STREQUAL "RELEASE")
        if(NOT ${SCOPE_RELEASE_ACTIVE})
            return()
        endif()
    endif()

    if(DEP_ARGS_DEPENDENCY_VERBOSE)
        set(FETCHCONTENT_QUIET OFF)
    endif()

    # Check if dependency resource was specified in a short form
    set(_DEP_INTEGRATION_URL_PATTERN
        "^[A-Za-z0-9]+[A-Za-z0-9-]*\/[A-Za-z0-9]+[A-Za-z0-9-]*$"
    )
    if(${DEP_ARGS_DEPENDENCY_RESOURCE} MATCHES ${_DEP_INTEGRATION_URL_PATTERN})
        set(_DEP_SRC_RES "${DEP_ARGS_DEPENDENCY_RESOURCE}.git")
        # Check if the environment overrides the default base URL to be used
        if(DEFINED ENV{PROJECTS_CMAKE_DEPENDENCIES_INTEGRATION_URL})
            set(_DEP_BASE_URL
                "$ENV{PROJECTS_CMAKE_DEPENDENCIES_INTEGRATION_URL}"
            )
        else()
            set(_DEP_BASE_URL "${DEPENDENCY_BASE_URL_DEFAULT}")
        endif()
        # Overwrite resource arg
        set(DEP_ARGS_DEPENDENCY_RESOURCE "${_DEP_BASE_URL}/${_DEP_SRC_RES}")
    endif()

    # Set default conventional config and build files
    string(
        TOLOWER
        ${DEP_ARGS_DEPENDENCY_NAME}
        DEPENDENCY_NAME_LOWER
    )
    string(
        SUBSTRING
        ${DEPENDENCY_NAME_LOWER} 1 -1
        DEPENDENCY_NAME_LOWER
    )
    string(
        SUBSTRING
        ${DEP_ARGS_DEPENDENCY_NAME} 0 1
        DEPENDENCY_NAME_FIRST
    )
    string(
        TOUPPER
        ${DEPENDENCY_NAME_FIRST}
        DEPENDENCY_NAME_FIRST
    )
    set(DEPENDENCY_DEFAULT_CONFIG_FILE "cmake/Config")
    string(APPEND DEPENDENCY_DEFAULT_CONFIG_FILE ${DEPENDENCY_NAME_FIRST})
    string(APPEND DEPENDENCY_DEFAULT_CONFIG_FILE ${DEPENDENCY_NAME_LOWER})
    string(APPEND DEPENDENCY_DEFAULT_CONFIG_FILE ".cmake")
    string(
        PREPEND
        DEPENDENCY_DEFAULT_CONFIG_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}" "/"
    )
    set(DEPENDENCY_DEFAULT_BUILD_FILE "cmake/Build")
    string(APPEND DEPENDENCY_DEFAULT_BUILD_FILE ${DEPENDENCY_NAME_FIRST})
    string(APPEND DEPENDENCY_DEFAULT_BUILD_FILE ${DEPENDENCY_NAME_LOWER})
    string(APPEND DEPENDENCY_DEFAULT_BUILD_FILE ".cmake")
    string(
        PREPEND
        DEPENDENCY_DEFAULT_BUILD_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}" "/"
    )

    set(OPT_DEP_CACHE_SRC_PATH "")
    set(OPT_DEP_GIT_SHALLOW "GIT_SHALLOW")
    set(DEP_USE_GIT_SHALLOW ON)
    set(DEP_CACHE_SRC_PATH "")
    set(DEP_CACHE_HINT_MSG "")

    # Check if the dependency version was specified as a git commit
    # hash, in which case we want to disable the Git shallow option.
    # CMake does not support regex quantifiers, so here we're matching
    # one or more hex digits instead of exactly 40 for the SHA-1 hash.
    if(${DEP_ARGS_DEPENDENCY_VERSION} MATCHES "^[0-9a-f]+$")
        set(OPT_DEP_GIT_SHALLOW "")
        set(DEP_USE_GIT_SHALLOW "")
    endif()

    # Check whether to consider the source cache
    if( NOT "$ENV{A_CMAKE_DEPENDENCY_NO_CACHE}" STREQUAL "1"
        AND NOT DEP_ARGS_DEPENDENCY_NO_CACHE
        AND DEP_ARGS_DEPENDENCY_VERSION
        AND DEFINED ENV{HOME})

        set(DEP_CACHE_PREFIX "$ENV{HOME}")
        if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.cache/cmake_deps_src")
            set(DEP_CACHE_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}")
        endif()

        set(DEP_CACHE_SRC_BASE "${DEP_CACHE_PREFIX}/.cache/cmake_deps_src")
        set(DEP_CACHE_SRC_UNIT
            "${DEP_ARGS_DEPENDENCY_NAME}/${DEP_ARGS_DEPENDENCY_VERSION}")

        set(OPT_DEP_CACHE_SRC_PATH SOURCE_DIR)
        set(DEP_CACHE_SRC_PATH "${DEP_CACHE_SRC_BASE}/${DEP_CACHE_SRC_UNIT}")
        file(TO_CMAKE_PATH "${DEP_CACHE_SRC_PATH}" DEP_CACHE_SRC_PATH)

        # Check if dependency sources are in the cache
        if(EXISTS "${DEP_CACHE_SRC_PATH}")
            if(DEP_ARGS_DEPENDENCY_VERBOSE)
                message(
                    STATUS
                    "Found cached dependency sources "
                    "at '${DEP_CACHE_SRC_PATH}'"
                )
            endif()
            set(FETCHCONTENT_SOURCE_DIR_${DEP_ARGS_DEPENDENCY_NAME_UPPER}
                "${DEP_CACHE_SRC_PATH}" CACHE STRING "" FORCE
            )
           set(DEP_CACHE_HINT_MSG "(cached sources)")
       endif()
    else()
        set(DEP_CACHE_HINT_MSG "(cache disabled)")
    endif()

    if(NOT DEP_ARGS_DEPENDENCY_QUIET)
        message(
            STATUS
            "Processing dependency ${DEP_ARGS_DEPENDENCY_NAME} "
            "${DEP_CACHE_HINT_MSG}"
        )
    endif()

    if(${DEP_ARGS_DEPENDENCY_RESOURCE} MATCHES "\.git$")
        # Git repository
        FetchContent_Declare(
            ${DEP_ARGS_DEPENDENCY_NAME}
            GIT_REPOSITORY            ${DEP_ARGS_DEPENDENCY_RESOURCE}
            GIT_TAG                   ${DEP_ARGS_DEPENDENCY_VERSION}
            ${OPT_DEP_GIT_SHALLOW}    ${DEP_USE_GIT_SHALLOW}
            ${OPT_DEP_CACHE_SRC_PATH} "${DEP_CACHE_SRC_PATH}"
        )
    else()
        if(${DEP_ARGS_DEPENDENCY_RESOURCE} MATCHES "^(http|https|file)://")
            # Source archive file
            if(NOT DEFINED DEP_ARGS_DEPENDENCY_FILE_HASH)
                message(
                    FATAL_ERROR
                    "Dependency ${DEP_ARGS_DEPENDENCY_NAME} has a missing "
                    "argument. Please provide argument DEPENDENCY_FILE_HASH "
                    "for archive file dependency resources."
                )
            endif()
            FetchContent_Declare(
                ${DEP_ARGS_DEPENDENCY_NAME}
                URL "${DEP_ARGS_DEPENDENCY_RESOURCE}"
                SOURCE_DIR "${DEP_CACHE_SRC_PATH}"
                URL_HASH SHA256=${DEP_ARGS_DEPENDENCY_FILE_HASH}
            )
        else()
            message(
                FATAL_ERROR
                "Dependency ${DEP_ARGS_DEPENDENCY_NAME} resource value "
                "must either represent a Git repository "
                "(ending with '.git'), or a file resource in either "
                "'.zip' or 'tar.gz' format accessible over HTTP(S) or as "
                "a local file resource."
            )
        endif()
    endif()

    # Check if a config file needs to be included
    if(NOT ${DEP_ARGS_DEPENDENCY_NO_CONFIG})
        if(DEFINED DEP_ARGS_DEPENDENCY_CONFIG_FILE)
            # Specific file provided by function arg
            set(DEPENDENCY_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/")
            string(
                APPEND
                DEPENDENCY_CONFIG_FILE
                "${DEP_ARGS_DEPENDENCY_CONFIG_FILE}"
            )
            if(EXISTS "${DEPENDENCY_CONFIG_FILE}")
                include("${DEPENDENCY_CONFIG_FILE}")
            else()
                message(
                    WARNING
                    "No configuration file found for "
                    "dependency ${DEPENDENCY_CONFIG_FILE}"
                )
            endif()
        else()
            # Use the conventional file
            if(EXISTS "${DEPENDENCY_DEFAULT_CONFIG_FILE}")
                include("${DEPENDENCY_DEFAULT_CONFIG_FILE}")
            endif()
        endif()
    endif()

    FetchContent_MakeAvailable(${DEP_ARGS_DEPENDENCY_NAME})

    # Check if a build file needs to be included
    if(DEFINED DEP_ARGS_DEPENDENCY_BUILD_FILE)
        set(
            DEPENDENCY_BUILD_FILE
            "${CMAKE_CURRENT_SOURCE_DIR}/${DEP_ARGS_DEPENDENCY_BUILD_FILE}"
        )
        if(EXISTS "${DEPENDENCY_BUILD_FILE}")
            include("${DEPENDENCY_BUILD_FILE}")
        else()
            message(
                WARNING
                "The specified build file for "
                "dependency ${DEP_ARGS_DEPENDENCY_NAME} "
                "was not found: '${DEPENDENCY_BUILD_FILE}'"
            )
        endif()
    else()
        # Use the conventional file
        if(EXISTS "${DEPENDENCY_DEFAULT_BUILD_FILE}")
            include("${DEPENDENCY_DEFAULT_BUILD_FILE}")
        endif()
    endif()

    # Check tracked identifiers
    if(DEPENDENCY_SCOPE IN_LIST DEP_TRACKED_SCOPES)
        # Append dependency link targets to a tracked global cached list
        if(DEP_ARGS_DEPENDENCY_LINK_TARGETS)
            set(PROJECT_DEPENDENCIES_LINK_TARGETS
                ${PROJECT_NAME_UPPER}_DEPENDENCIES_LINK_TARGETS
            )
            set(_dep_link_list "${${PROJECT_DEPENDENCIES_LINK_TARGETS}}")
            foreach(_dep_link_item IN ITEMS ${DEP_ARGS_DEPENDENCY_LINK_TARGETS})
                list(FIND _dep_link_list "${_dep_link_item}" _dep_idx)
                if(_dep_idx EQUAL -1)
                    list(APPEND _dep_link_list "${_dep_link_item}")
                endif()
            endforeach()
            set(${PROJECT_DEPENDENCIES_LINK_TARGETS}
                ${_dep_link_list}
                CACHE STRING
                "List of link targets of all dependencies"
                FORCE
            )
        endif()

        # Append dependency name to a tracked global cached list
        set(PROJECT_DEPENDENCIES_LIST ${PROJECT_NAME_UPPER}_DEPENDENCIES_LIST)
        set(_dep_list "${${PROJECT_DEPENDENCIES_LIST}}")
        list(FIND _dep_list "${DEP_ARGS_DEPENDENCY_NAME}" _dep_idx)
        if(_dep_idx EQUAL -1)
            list(APPEND _dep_list "${DEP_ARGS_DEPENDENCY_NAME}")
            set(
                ${PROJECT_DEPENDENCIES_LIST}
                "${_dep_list}"
                CACHE STRING
                "List of dependency names (scopes ANY/RELEASE/DEBUG)"
                FORCE
            )
        endif()
    endif()
endfunction()
