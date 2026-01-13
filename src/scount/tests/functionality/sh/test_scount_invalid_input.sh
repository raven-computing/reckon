#!/bin/bash
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


function test_executing_scount_with_no_arguments_prints_usage() {
  run_app;
  assert_exit_status $EXIT_INVALID_ARGUMENT;
  assert_stderr_is_empty;
  assert_stdout_contains "No input path specified.";
  assert_stdout_contains "Try 'scount --help' for more information.";
  assert_stdout_contains "Usage: scount ";
}

function test_scount_with_invalid_option_argument() {
  run_app --this-is-not-valid;
  assert_exit_status $EXIT_INVALID_ARGUMENT;
  assert_stdout_is_empty;
  assert_stderr_equals "Unknown option: '--this-is-not-valid'";
}

function test_executing_scount_with_nonexistent_file_input_prints_error() {
  local invalid_path="${TEST_PROJECT_DIR}/this-file-does-not-exist";
  run_app "$invalid_path";
  assert_exit_status $EXIT_INVALID_INPUT;
  assert_stdout_is_empty;
  assert_stderr_equals "Invalid input path: '${invalid_path}'${_NL}" \
                       "No such file or directory (0x02)";
}

function test_executing_scount_with_multiple_path_arguments_prints_error() {
  local base_path="${TEST_PROJECT_DIR}/src";
  run_app "${base_path}/lib/tests/res" "${base_path}/src/scount/tests/res";
  assert_exit_status $EXIT_INVALID_ARGUMENT;
  assert_stderr_equals "Multiple input paths specified.";
  assert_stdout_is_empty;
}

function test_scount_prints_error_when_annotating_source_of_nonexistent_file() {
  run_app --annotate-counts "${TEST_PROJECT_DIR}/this-file-does-not-exist";
  assert_exit_status $EXIT_INVALID_INPUT;
  assert_stdout_is_empty;
  assert_stderr_contains "Failed to annotate source file";
  assert_stderr_contains "Check the logical line count";
}

function test_scount_prints_error_when_annotating_source_of_directory() {
  run_app --annotate-counts "${TEST_PROJECT_DIR}/lib/tests/res";
  assert_exit_status $EXIT_INVALID_INPUT;
  assert_stdout_is_empty;
  assert_stderr_contains "Failed to annotate source file";
  assert_stderr_contains "Check the logical line count";
}
