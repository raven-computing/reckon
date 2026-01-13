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
# Contains functions providing build utilities.
# The minimum CMake version required by this code is v3.22.
#
#==============================================================================

# Enables all compiler warnings for the specified target.
#
# Sets non-default warning flags for the given target to help identify
# potential issues during compilation. The `inform_only` argument specifies
# whether all warnings should be treated as errors (`OFF`) or whether warnings
# should only be shown but the build should not fail (`ON`).
#
# Arguments:
#
#   target_name:
#       The name of the target to enable all compiler warnings for.
#
#   inform_only:
#       Whether to treat warnings as errors or only inform about them and
#       not cause the build to fail.
#
# Example:
#   enable_compiler_warnings(mytarget OFF)
#
function(enable_compiler_warnings target_name inform_only)
    set(WARN_AS_ERROR ON)
    if(inform_only STREQUAL "ON")
        set(WARN_AS_ERROR OFF)
    endif()

    set(
        FLAGS_GCC
        "-Wall" "-Wextra" "-pedantic"
        "-Wno-unused-parameter" "-Wno-unused-function"
    )
    set(FLAGS_MSVC "/W4" "/permissive-" "/wd4100")
    target_compile_options(
        ${target_name}
        PRIVATE
        $<$<C_COMPILER_ID:GNU>:${FLAGS_GCC}>
        $<$<C_COMPILER_ID:MSVC>:${FLAGS_MSVC}>
        $<$<CXX_COMPILER_ID:GNU>:${FLAGS_GCC}>
        $<$<CXX_COMPILER_ID:MSVC>:${FLAGS_MSVC}>
    )

    if(WARN_AS_ERROR)
        set(FLAGS_GCC "-Werror")
        set(FLAGS_MSVC "/WX")
        target_compile_options(
            ${target_name}
            PRIVATE
            $<$<C_COMPILER_ID:GNU>:${FLAGS_GCC}>
            $<$<C_COMPILER_ID:MSVC>:${FLAGS_MSVC}>
            $<$<CXX_COMPILER_ID:GNU>:${FLAGS_GCC}>
            $<$<CXX_COMPILER_ID:MSVC>:${FLAGS_MSVC}>
        )
    endif()
endfunction()

# Disables all compiler warnings for the specified target.
#
# Arguments:
#
#   target_name:
#       The name of the target to disable all compiler warnings for.
#
# Example:
#   disable_compiler_warnings(mytarget)
#
function(disable_compiler_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W0)
    else()
        target_compile_options(${target_name} PRIVATE -w)
    endif()
endfunction()

# Enables compile-time source code checks for a given CMake target.
#
# Appends the appropriate compiler flags, if available, to let the compiler
# perform static analysis on the source code at compile time.
#
# This feature is currently only available when compiling with GCC and only
# applicable for C source code. For source files written in C++, this feature
# has currently no effect.
#
# Arguments:
#
#   target_name:
#       The name of the target for which to enable source code checks.
#       This argument is mandatory.
#
# Example:
#   enable_source_compile_checks(mytarget)
#
function(enable_source_compile_checks target_name)
    if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
        message(
            WARNING
            "Cannot enable compile-time source code checks. "
            "This is only available when using GCC."
        )
        return()
    endif()

    target_compile_options(
        ${target_name}
        PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-fanalyzer>
    )

endfunction()

# Sets the output directories for all build artifacts.
#
# Sets the global CMAKE_*_OUTPUT_DIRECTORY variables to default locations
# within the build directory, unless they have already been defined.
# Also applies the same for multi-configuration generators by setting the
# CMAKE_*_OUTPUT_DIRECTORY_<CONFIG> variables. The variables are set in the
# scope of the caller.
#
function(set_output_directories)
    # Multi-configuration generators
    foreach(OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG_UPPER)
        if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER})
            set(
                CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}
                "${CMAKE_BINARY_DIR}/lib/${OUTPUT_CONFIG}"
                PARENT_SCOPE
            )
        endif()
        if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER})
            set(
                CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}
                "${CMAKE_BINARY_DIR}/lib/${OUTPUT_CONFIG}"
                PARENT_SCOPE
            )
        endif()
        if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER})
            set(
                CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG_UPPER}
                "${CMAKE_BINARY_DIR}/bin/${OUTPUT_CONFIG}"
                PARENT_SCOPE
            )
        endif()
    endforeach()
    # Global settings
    if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        set(
            CMAKE_ARCHIVE_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/lib"
            PARENT_SCOPE
        )
    endif()
    if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(
            CMAKE_LIBRARY_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/lib"
            PARENT_SCOPE
        )
    endif()
    if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(
            CMAKE_RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/bin"
            PARENT_SCOPE
        )
    endif()
endfunction()

# Sets a target to be stripped when building on Unix-like systems.
#
# Appends the appropriate linker options to strip the given target.
#
# Arguments:
#
#   target_name:
#       The name of the target that should be stripped when building.
#       This argument is mandatory.
#
# Example:
#   set_stripped_executable(mytarget)
#
function(set_stripped_executable target_name)
    set(platform "$<BOOL:${UNIX}>")
    set(variant "$<CONFIG:Release>")
    set(gcc_or_clang "$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>")
    set(strip_opt "-Wl,--strip-all")
    target_link_options(
        ${target_name}
        PRIVATE
        "$<$<AND:${platform},${variant},${gcc_or_clang}>:${strip_opt}>"
    )

endfunction()
