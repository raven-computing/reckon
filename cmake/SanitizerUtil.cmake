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
# Contains a function for adding compiler and linker options to targets in
# CMake-based projects to add support for sanitizers, for example address
# and memory leak sanitizers.
# The minimum CMake version required by this code is v3.22.
#
#==============================================================================

# Adds sanitizer support to a given CMake target.
#
# Appends the appropriate compiler and linker flags to enable sanitizers for
# the specified target.
#
# Arguments:
#
#   target_name:
#       The name of the target to which sanitizers will be added.
#       This argument is mandatory.
#
# Example:
#   add_sanitizers(mytarget)
#
function(add_sanitizers target_name)
    set(
        SAN_COMPILE_FLAGS_LINUX
        "-fsanitize=address" "-fsanitize=leak" "-fsanitize=undefined"
        "-fno-omit-frame-pointer"
    )
    set(
        SAN_LINK_FLAGS
        "-fsanitize=address" "-fsanitize=leak" "-fsanitize=undefined"
    )
    set(SAN_COMPILE_FLAGS_WINDOWS "/fsanitize=address" "/Oy-")

    target_compile_options(
        ${target_name}
        PUBLIC
        $<$<PLATFORM_ID:Linux>:${SAN_COMPILE_FLAGS_LINUX}>
        $<$<PLATFORM_ID:Windows>:${SAN_COMPILE_FLAGS_WINDOWS}>
    )
    target_link_options(
        ${target_name}
        PUBLIC
        $<$<NOT:$<PLATFORM_ID:Windows>>:${SAN_LINK_FLAGS}>
    )

endfunction()
