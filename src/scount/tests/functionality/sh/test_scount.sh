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


TEST_RES_DIR="${TEST_PROJECT_DIR}/src/scount/tests/functionality/res";


function test_scount_prints_correct_output_for_single_file_input() {
  run_app "${TEST_PROJECT_DIR}/src/lib/tests/res/java/Sample.java";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals_file "expected/output_single_file.txt";
  assert_stderr_is_empty;
}

function test_scount_prints_correct_output_for_directory_input() {
  run_app "${TEST_PROJECT_DIR}/src/lib/tests/res/java";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals_file "expected/output_multiple_files.txt";
  assert_stderr_is_empty;
}

function test_scount_with_relative_file_path_input() {
  run_app "src/lib/tests/res/java/Sample.java";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals_file "expected/output_single_file.txt";
  assert_stderr_is_empty;
}

function test_scount_with_relative_directory_input() {
  run_app "src/lib/tests/res/java";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals_file "expected/output_multiple_files.txt";
  assert_stderr_is_empty;
}

function test_scount_prints_annotated_source_code() {
  run_app --annotate-counts "${TEST_PROJECT_DIR}/src/lib/tests/res/java/Sample.java";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals_file "expected/AnnotatedSource.java";
  assert_stderr_is_empty;
}

function test_scount_with_directory_input_which_contains_no_eligible_files() {
  run_app "${TEST_RES_DIR}/NotEligible";
  assert_exit_status $EXIT_NOTHING_PROCESSED;
  assert_stdout_is_empty;
  assert_stderr_contains "Scanned directory";
  assert_stderr_contains "It contains no files with an eligible format";
}

function test_scount_with_not_eligible_file_input() {
  run_app "${TEST_RES_DIR}/NotEligible/file.withUnknownExt";
  assert_exit_status $EXIT_NOTHING_PROCESSED;
  assert_stdout_is_empty;
  assert_stderr_contains "Scanned file";
  assert_stderr_contains "The file 'file.withUnknownExt' cannot be processed";
  assert_stderr_contains "It does not have an eligible format.";
  assert_stderr_contains "The file extension is not supported: 'withUnknownExt'";
}

function test_scount_with_file_input_without_file_extension() {
  run_app "${TEST_RES_DIR}/NotEligible/fileWithoutExt";
  assert_exit_status $EXIT_NOTHING_PROCESSED;
  assert_stdout_is_empty;
  assert_stderr_contains "Scanned file"; 
  assert_stderr_contains "The file 'fileWithoutExt' cannot be processed";
  assert_stderr_contains "It does not have an eligible format.";
}

function test_scount_with_directory_containing_one_non_eligible_file() {
  run_app "src/scount/tests/functionality/res/DirWithOneNonEligibleFile";
  assert_exit_status $EXIT_NOTHING_PROCESSED;
  assert_stdout_is_empty;
  assert_stderr_contains "Scanned directory"; 
  assert_stderr_contains "src/scount/tests/functionality/res/DirWithOneNonEligibleFile";
  assert_stderr_contains "The file 'file.withUnknownExt' cannot be processed.";
  assert_stderr_contains "It does not have an eligible format. ";
  assert_stderr_contains "The file extension is not supported: 'withUnknownExt'";
}

function test_scount_with_mixed_directory_content() {
  run_app "${TEST_RES_DIR}/mixed";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals_file "expected/mixed.txt";
  assert_stderr_is_empty;
}

function test_scount_directory_input_with_trailing_path_separator() {
  run_app "${TEST_RES_DIR}/mixed/";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals_file "expected/mixed.txt";
  assert_stderr_is_empty;
}

function test_scount_prints_verbose_output() {
  local test_path="${TEST_PROJECT_DIR}/src/lib/tests/res/java";
  run_app --verbose "$test_path";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_contains "Processing input path:";
  assert_stdout_contains "'${test_path}'";
  assert_stdout_contains "A total of 3 files found";
  assert_stdout_contains "Found file: '${test_path}/Sample.java' (status: 0x0000)";
  assert_stdout_contains "Found file: '${test_path}/SampleAnnotated.java' (status: 0x0000)";
  assert_stdout_contains "Found file: '${test_path}/SampleMinFormatting.java' (status: 0x0000)";
  assert_stderr_is_empty;
}

function test_scount_with_directory_that_contains_file_with_syntax_error() {
  run_app "${TEST_RES_DIR}/mixedWithSyntaxError";
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals_file "expected/mixedWithSyntaxError.txt";
  assert_stderr_is_empty;
}

function test_scount_with_file_that_has_syntax_error() {
  local file="${TEST_RES_DIR}/mixedWithSyntaxError/02_has_syntax_error.c";
  run_app "$file";
  assert_exit_status $EXIT_INVALID_INPUT;
  assert_stdout_is_empty;
  assert_stderr_contains "An error has occurred for: ";
  assert_stderr_contains "${file}";
  assert_stderr_contains "Syntax error detected in source code (0x04)";
}

function test_scount_with_stop_on_error_option() {
  run_app --stop-on-error "${TEST_RES_DIR}/mixedWithSyntaxError";
  assert_exit_status $EXIT_INVALID_INPUT;
  assert_stdout_is_empty;
  assert_stderr_contains "An error has occurred";
  assert_stderr_contains "Syntax error detected in source code";
}
