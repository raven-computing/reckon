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
# Defines a function for removing coverage data files created during
# test coverage collection in CMake-based projects.
# This file may be run in CMake script mode, which will automatically execute
# the cleanup_code_coverage_collection_data() function.
# The minimum CMake version required by this code is v3.22.
#
#==============================================================================

# Removes code coverage data files created during test coverage collection.
#
# Cleans up coverage data files and directories inside the build tree as
# defined by the CMAKE_BINARY_DIR variable. When run in script mode, make sure
# that variable and the current working directory are set correctly.
#
# Usage:
#   cleanup_code_coverage_collection_data()
#
function(cleanup_code_coverage_collection_data)
    file(
        REMOVE_RECURSE
        "${CMAKE_BINARY_DIR}/cov"
        "${CMAKE_BINARY_DIR}/coverage_report"
    )
    file(GLOB_RECURSE GCDA_FILES "${CMAKE_BINARY_DIR}/*.gcda")
    if(GCDA_FILES)
        file(REMOVE ${GCDA_FILES})
    endif()

endfunction()

if(CMAKE_SCRIPT_MODE_FILE)
    cleanup_code_coverage_collection_data()
endif()
