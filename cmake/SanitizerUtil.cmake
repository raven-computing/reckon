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
# Contains a function for adding compiler and linker options to targets in
# CMake-based projects to add support for sanitizers, for example address
# and memory leak sanitizers.
# The minimum CMake version required by this code is v3.22.
#
#==============================================================================

# Adds sanitizer support to a given CMake target.
#
# Appends the appropriate compiler and linker flags to enable sanitizers for
# the specified target. Supported platforms are GNU/Linux and Windows.
# Sanitizers can be used for release build variants but preferably are only
# enabled for debug builds to get the most informative output.
#
# Arguments:
#
#   target_name:
#       The name of the target to which sanitizers will be added.
#       This argument is mandatory.
#
#   USE_THREAD_SANITIZER:
#       Optional argument. If specified, enables thread sanitizer (TSAN)
#       instead of the default Address/Leak/Undefined sanitizers.
#       This option can only be used on Linux.
#
# Example:
#   add_sanitizers(mytarget)
#   add_sanitizers(mytarget USE_THREAD_SANITIZER)
#
function(add_sanitizers target_name)

    cmake_parse_arguments(ARG "USE_THREAD_SANITIZER" "" "" ${ARGN})

    if(ARG_USE_THREAD_SANITIZER)
        if(MSVC)
            message(
                WARNING
                "Thread sanitizer is not supported when using MSVC. "
                "Falling back to default sanitizers."
            )
            set(ARG_USE_THREAD_SANITIZER FALSE)
        endif()
    endif()

    if(ARG_USE_THREAD_SANITIZER)
        set(
            COMP_FLAGS_GNU
            "-fsanitize=thread"
            "-fno-omit-frame-pointer"
        )
        set(
            LINK_FLAGS_GNU
            "-fsanitize=thread"
        )
    else()
        set(
            COMP_FLAGS_GNU
            "-fsanitize=address" "-fsanitize=leak" "-fsanitize=undefined"
            "-fno-omit-frame-pointer"
        )
        set(
            LINK_FLAGS_GNU
            "-fsanitize=address" "-fsanitize=leak" "-fsanitize=undefined"
        )
    endif()

    set(COMP_FLAGS_MSVC "/fsanitize=address" "/Oy-" "/Zi")
    set(LINK_FLAGS_MSVC "/INCREMENTAL:NO")

    target_compile_options(
        ${target_name}
        PUBLIC
        $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:${COMP_FLAGS_GNU}>
        $<$<C_COMPILER_ID:MSVC>:${COMP_FLAGS_MSVC}>
    )
    target_link_options(
        ${target_name}
        PUBLIC
        $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:${LINK_FLAGS_GNU}>
        $<$<C_COMPILER_ID:MSVC>:${LINK_FLAGS_MSVC}>
    )

    message(STATUS "Sanitizer support enabled for target ${target_name}")

endfunction()
