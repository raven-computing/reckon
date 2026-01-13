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
# This file contains a function for specifying test suites in
# a declarative way. Eventually, a test suite is registered with CTest by
# making a call to the add_test() function provided by CTest. The utility
# function in this file takes care of the boilerplate setup required
# to register a new test. The minimum CMake version required by
# this code is v3.22.
#
#==============================================================================

include(CTest)

# This function is used to register a test suite with CTest. It can be used
# in two ways, which depends on the specified source file for the test suite.
# It is assumed that the source code of both the application and the tests are
# organised in the standard directory structure.
#
# If the specified source is a C source file, a new target will be created for
# the test executable and it will be passed to CTest's add_test() function.
# The created test executable is automatically linked to unity (the used
# testing framework for C projects). It is assumed that the unity dependency
# is available to the underlying project.
#
# If the specified source is a shell script, the functionality test wrapper
# script is used as the test driver program and the specified shell script is
# passed as the first argument to it. In this case, the link target must be
# specified and it must be an executable target type.
#
# Arguments:
#
#   [oneValueArgs]
#
#   TEST_SUITE_NAME:
#       The name of the test suite to be created. This is the name by which
#       the test can be identified when running all tests via
#       the ctest command. This argument is mandatory.
#
#   TEST_SUITE_TARGET:
#       The target name for the test executable to be created. This argument
#       is passed on to the add_executable() function.
#       This argument is mandatory.
#
#   TEST_SUITE_SOURCE:
#       The path to the source files of the test suite.
#       This argument is mandatory.
#
#   TEST_SUITE_LINK:
#       The target that the test executable should be linked against.
#       This argument is optional when the source file is a C source file,
#       but it is mandatory when the source file is a shell script.
#
#   ENV_VAR_RECKON_DEBUG:
#       The value of the RECKON_DEBUG environment variable to set.
#
# Example usage:
#
# add_test_suite(
#     TEST_SUITE_NAME      MyTestSuite
#     TEST_SUITE_TARGET    test_mylib
#     TEST_SUITE_SOURCE    c/test_myfile.c
#     TEST_SUITE_LINK      mylibtarget
# )
#
function(add_test_suite)

    set(oneValueArgs
        TEST_SUITE_NAME
        TEST_SUITE_TARGET
        TEST_SUITE_SOURCE
        TEST_SUITE_LINK
        ENV_VAR_RECKON_DEBUG
    )
    set(multiValueArgs "")

    cmake_parse_arguments(
        TEST_SUITE_ARGS
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # Check mandatory arguments
    if(NOT DEFINED TEST_SUITE_ARGS_TEST_SUITE_NAME)
        message(
            FATAL_ERROR
            "Missing argument 'TEST_SUITE_NAME' in call "
            "to add_test_suite() function"
        )
    endif()

    if(NOT DEFINED TEST_SUITE_ARGS_TEST_SUITE_TARGET)
        message(
            FATAL_ERROR
            "Missing argument 'TEST_SUITE_TARGET' in call "
            "to add_test_suite() function"
        )
    endif()

    if(NOT DEFINED TEST_SUITE_ARGS_TEST_SUITE_SOURCE)
        message(
            FATAL_ERROR
            "Missing argument 'TEST_SUITE_SOURCE' in call "
            "to add_test_suite() function"
        )
    endif()

    # Link against the unity target
    set(TEST_SUITE_UNITY_LINK_ARGS unity)
    set(TEST_SUITE_COMMAND_ARGS "")
    set(TEST_SUITE_TARGET_NAME "")
    set(TEST_SUITE_IS_SHELL_BASED False)
    set(ENV_VAR_RECKON_DEBUG "0")


    if(DEFINED TEST_SUITE_ARGS_ENV_VAR_RECKON_DEBUG)
        set(
            ENV_VAR_RECKON_DEBUG
            "${TEST_SUITE_ARGS_ENV_VAR_RECKON_DEBUG}"
        )
    endif()

    if (TEST_SUITE_ARGS_TEST_SUITE_SOURCE MATCHES ".*\.sh$")
        # Is a non-compiled functionality test case with a shell wrapper
        set(TEST_SUITE_IS_SHELL_BASED True)
        if(NOT DEFINED TEST_SUITE_ARGS_TEST_SUITE_LINK)
            message(
                FATAL_ERROR
                "Missing argument 'TEST_SUITE_LINK' "
                "in call to add_test_suite() function. "
                "When the test suite source argument is a functionality test "
                "shell script, the 'TEST_SUITE_LINK' argument must be "
                "specified as the target of the executable under test."
            )
        endif()
        get_target_property(
            LINK_TARGET_TYPE
            ${TEST_SUITE_ARGS_TEST_SUITE_LINK}
            TYPE
        )
        if (NOT LINK_TARGET_TYPE STREQUAL "EXECUTABLE")
            message(
                FATAL_ERROR
                "Invalid argument 'TEST_SUITE_LINK' in call "
                "to add_test_suite() function: "
                "When the test suite source argument is a functionality test "
                "shell script, the link target must be an executable target "
                "type, but the specified "
                "target '${TEST_SUITE_ARGS_TEST_SUITE_LINK}' "
                "is of type ${LINK_TARGET_TYPE}"
            )
        endif()
        set(TEST_SUITE_TARGET_NAME "${TEST_SUITE_ARGS_TEST_SUITE_TARGET}")
        # Replace the target with the functionality test driver script
        set(
            TEST_SUITE_ARGS_TEST_SUITE_TARGET
            bash "${CMAKE_CURRENT_SOURCE_DIR}/functionality/sh/driver.sh"
        )
        # The actual shell script containing the test suite is passed as
        # the first command argument to the test driver script
        set(
            TEST_SUITE_COMMAND_ARGS
            "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SUITE_ARGS_TEST_SUITE_SOURCE}"
        )
    else()
        # Create a compiled test target
        add_executable(
            ${TEST_SUITE_ARGS_TEST_SUITE_TARGET}
            ${TEST_SUITE_ARGS_TEST_SUITE_SOURCE}
        )

        # Include source header files
        target_include_directories(
            ${TEST_SUITE_ARGS_TEST_SUITE_TARGET}
            PRIVATE
            ../c
        )

        set_target_properties(
            ${TEST_SUITE_ARGS_TEST_SUITE_TARGET}
            PROPERTIES
            C_STANDARD ${RECKON_C_STANDARD}
            C_STANDARD_REQUIRED True
        )

        # Reckon libraries are part of the test executable
        target_compile_definitions(
            ${TEST_SUITE_ARGS_TEST_SUITE_TARGET}
            PRIVATE
            RECKON_STATIC_DEFINE
        )

        # Link against unity dependency targets,
        # and user provided ones
        target_link_libraries(
            ${TEST_SUITE_ARGS_TEST_SUITE_TARGET}
            ${TEST_SUITE_ARGS_TEST_SUITE_LINK}
            ${TEST_SUITE_UNITY_LINK_ARGS}
        )
    endif()

    # Register the test suite
    add_test(
        NAME ${TEST_SUITE_ARGS_TEST_SUITE_NAME}
        COMMAND
        ${TEST_SUITE_ARGS_TEST_SUITE_TARGET}
        ${TEST_SUITE_COMMAND_ARGS}
        WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}
    )

    # Add label to test so test script can filter by it
    if (TEST_SUITE_ARGS_TEST_SUITE_SOURCE
        MATCHES "^(unit|integration|functionality)\/.*")

        # Capitalise first letter of label
        set(TEST_SUITE_LABEL_LOWER ${CMAKE_MATCH_1})
        string(SUBSTRING ${TEST_SUITE_LABEL_LOWER} 0 1 TEST_SUITE_LABEL_FIRST)
        string(TOUPPER ${TEST_SUITE_LABEL_FIRST} TEST_SUITE_LABEL_FIRST)
        string(SUBSTRING ${TEST_SUITE_LABEL_LOWER} 1 -1 TEST_SUITE_LABEL_LOWER)
        set(TEST_SUITE_LABEL ${TEST_SUITE_LABEL_FIRST})
        string(APPEND TEST_SUITE_LABEL ${TEST_SUITE_LABEL_LOWER})
        set_property(
            TEST
            ${TEST_SUITE_ARGS_TEST_SUITE_NAME}
            PROPERTY
            LABELS "${TEST_SUITE_LABEL}"
        )
    endif()

    if (TEST_SUITE_IS_SHELL_BASED)
        set_property(
            TEST
            ${TEST_SUITE_ARGS_TEST_SUITE_NAME}
            PROPERTY
            ENVIRONMENT
            TEST_TARGET_APP=$<TARGET_FILE:${TEST_SUITE_ARGS_TEST_SUITE_LINK}>;
            TEST_TARGET_NAME=${TEST_SUITE_TARGET_NAME}
            TEST_PROJECT_DIR=${CMAKE_SOURCE_DIR}
            RECKON_DEBUG=${ENV_VAR_RECKON_DEBUG}
        )
    endif()

endfunction()
