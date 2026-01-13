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

# Common testing utilities and driver functionality. Provides functions to be used
# by functionality test cases to execute and test a given program end-to-end. Various
# assert_*() functions are provided to test an application's behaviour. This script
# can be safely run and sourced. When run without arguments, it does nothing. To use
# it as a test case driver, specify the path to the test script as the first argument.
# That test script is then automatically sourced and all test_*() functions in it are
# called sequentially. The TEST_TARGET_APP environment variable must be set to the path
# of the executable under test. The TEST_TARGET_NAME environment variable must be set
# to the name of the test target. The TEST_PROJECT_DIR environment variable must be
# set to the absolute path of the project source tree.

# Exit status codes as defined by the scount app
readonly EXIT_SUCCESS=0;
readonly EXIT_INVALID_ARGUMENT=1;
readonly EXIT_INVALID_INPUT=2;
readonly EXIT_NOTHING_PROCESSED=3;
readonly EXIT_PROG_IO_ERROR=4;
readonly EXIT_UNSPECIFIED_ERROR=126;

# The exit status code of this script if a test failure occurs, e.g. a failed assertion
readonly EXIT_TEST_FAILURE=1;
# The exit status code of this script if a test error occurs
readonly EXIT_TEST_ERROR=2;
# The exit status code of this script if an error occurs that is not a test failure
readonly EXIT_TEST_DRIVER_ERROR=64;

# Set to the exit status of the executable under test
TEST_TARGET_APP_EXIT_STATUS=0;
# The path to the file that contains the captured output of stdout
TEST_TARGET_FILE_STDOUT="";
# The path to the file that contains the captured output of stderr
TEST_TARGET_FILE_STDERR="";

# Platform-dependent newline character.
if [ -n "$MSYSTEM" ]; then
  readonly _NL=$'\r\n';
else
  readonly _NL=$'\n';
fi

# Original value of the IFS to be used for restore operations.
readonly _IFS_RESET="$IFS";

# Output variable of the _read_file() function.
READ_FILE_CONTENT="";

# Terminal colors
COLOR_RED="\033[1;31m";
COLOR_GREEN="\033[1;32m";
COLOR_BLUE="\033[1;34m";
COLOR_CYAN="\033[1;36m";
COLOR_ORANGE="\033[1;33m";
COLOR_NC="\033[0m";

# Honour standard environment variable
if [[ "$A_TERMINAL_USE_ANSI_COLORS" == "0" ]]; then
  COLOR_RED="";
  COLOR_GREEN="";
  COLOR_BLUE="";
  COLOR_CYAN="";
  COLOR_ORANGE="";
  COLOR_NC="";
fi

# Prints an assertion_failure message.
function _assertion_failure() {
  local message="$*";
  echo -n -e "${COLOR_RED}Assertion failure:${COLOR_NC} ";
  echo "$message";
  echo "";
}

# Prints a separator line with the specified length.
function _print_sep() {
  local cnt="$1";
  for (( i=0; i<cnt; ++i )); do
    echo -n "-";
  done
}

# Prints the file name and line number of a test failure.
# Should only be used by assert_*() functions directly.
function _print_test_failure_loc() {
  local test_src_assertion="${FUNCNAME[1]}";
  local test_src_ln="${BASH_LINENO[1]}";
  local test_src_func="${FUNCNAME[2]}";
  local test_src_file="$(basename ${BASH_SOURCE[2]})";
  echo -ne "${COLOR_RED}o";
  _print_sep 30;
  echo -ne " [Test Failure] ";
  _print_sep 29;
  echo -e "${COLOR_NC}";
  echo -e "${COLOR_RED}|${COLOR_NC} for assertion: ${test_src_assertion}()";
  echo -e "${COLOR_RED}|${COLOR_NC} at line:       ${test_src_ln}";
  echo -e "${COLOR_RED}|${COLOR_NC} in function:   ${test_src_func}()";
  echo -e "${COLOR_RED}|${COLOR_NC} in file:       ${test_src_file}";
  echo "";
}

# Internal function.
function _print_expected() {
  echo -ne "${COLOR_CYAN}";
  _print_sep 33;
  echo -ne " Expected ";
  _print_sep 33;
  echo -e "${COLOR_NC}";
  echo "$1";
  echo -ne "${COLOR_CYAN}";
  _print_sep 76;
  echo -e "${COLOR_NC}";
}

# Internal function.
function _print_actual() {
  echo -ne "${COLOR_RED}";
  _print_sep 34;
  echo -ne " Actual ";
  _print_sep 34;
  echo -e "${COLOR_NC}";
  echo "$1";
  echo -ne "${COLOR_RED}";
  _print_sep 76;
  echo -e "${COLOR_NC}";
}

# Prints a comparison of expected and actual program output.
# Should only be used by assert_*() functions directly.
function _print_diff() {
  _print_expected "$1";
  echo "";
  _print_actual "$2";
}

# Reads the given file and assigns the content to
# the READ_FILE_CONTENT global variable as is.
function _read_file() {
  local arg_file="$1";
  READ_FILE_CONTENT=$(cat "$arg_file" && echo -n EOF);
  if (( $? != 0 )); then
    echo "Error: Failed to read file '${arg_file}'";
    exit $EXIT_TEST_ERROR;
  fi
  READ_FILE_CONTENT="${READ_FILE_CONTENT%EOF}";
}

# Executes the application under test.
#
# The TEST_TARGET_APP environment variable must be set to the path
# of the executable to run.
#
# Args:
# $@ - The arguments forwarded to the executable.
#
function run_app() {
  "$TEST_TARGET_APP" $@ 1>"$TEST_TARGET_FILE_STDOUT" 2>"$TEST_TARGET_FILE_STDERR";
  TEST_TARGET_APP_EXIT_STATUS=$?;
}

# Asserts that the executable under test has returned the specified exit status.
#
# Args:
# $1 - The expected exit status to check for. This is a mandatory argument.
#
function assert_exit_status() {
  local expected_exit_status="$1";
  local actual_exit_status="$TEST_TARGET_APP_EXIT_STATUS";
  if (( actual_exit_status != expected_exit_status )); then
    _print_test_failure_loc;
    _assertion_failure "Expected exit status ${expected_exit_status}" \
                       "but was ${actual_exit_status}";
    exit $EXIT_TEST_FAILURE;
  fi
  return $EXIT_SUCCESS;
}

# Asserts that the executable under test has produced the specified output on stdout.
#
# Args:
# $* - The expected output of the executable. This is a mandatory argument.
#
function assert_stdout_equals() {
  local IFS='';
  local expected_output="$*";
  IFS="${_IFS_RESET}";
  _read_file "$TEST_TARGET_FILE_STDOUT";
  local actual_output="$READ_FILE_CONTENT";
  if [[ "$actual_output" != "$expected_output" ]]; then
    _print_test_failure_loc;
    _assertion_failure "Program output (stdout) does not match expected output";
    _print_diff "$expected_output" "$actual_output";
    exit $EXIT_TEST_FAILURE;
  fi
  return $EXIT_SUCCESS;
}

# Asserts that the executable under test has produced a specific output on stdout.
#
# The expected output is indicated by the specified file. The actual output of the
# application is compared with the content of that file. The specified path is
# relative to any test resource directory.
#
# Args:
# $1 - The file path where the expected output should be loaded from.
#      This is a mandatory argument.
#
function assert_stdout_equals_file() {
  local file="$1";
  local file_path="${TEST_PROJECT_DIR}/src/scount/tests/functionality/res/${file}";
  if ! [ -f "$file_path" ]; then
    file_path="${TEST_PROJECT_DIR}/src/lib/tests/res/${file}";
  fi
  if ! [ -f "$file_path" ]; then
    echo "Error: The file '${file}' was not found in any test source directory";
    exit $EXIT_TEST_ERROR;
  fi
  local expected_hash="";
  local actual_hash="";
  expected_hash=$(sha256sum "$file_path" |cut -d ' ' -f1);
  actual_hash=$(sha256sum "$TEST_TARGET_FILE_STDOUT" |cut -d ' ' -f1);
  if [[ "$expected_hash" != "$actual_hash" ]]; then
    _read_file "$file_path";
    local expected_output="$READ_FILE_CONTENT";
    _read_file "$TEST_TARGET_FILE_STDOUT";
    local actual_output="$READ_FILE_CONTENT";
    _print_test_failure_loc;
    _assertion_failure "Program output (stdout) does not match expected output";
    if (( ${#expected_output} > 1024 )); then
      echo "Computed SHA256 hashes differ:";
      echo "Expected: '${expected_hash}'";
      echo "Actual:   '${actual_hash}'";
      echo "";
    fi
    _print_diff "$expected_output" "$actual_output";
    exit $EXIT_TEST_FAILURE;
  fi
  return $EXIT_SUCCESS;
}

# Asserts that the executable under test has produced output on stdout that
# contains the specified text.
#
# Args:
# $1 - The expected text that must be part of the executable's stdout output.
#      This is a mandatory argument.
#
function assert_stdout_contains() {
  local expected_contains="$1";
  local actual_output="$(cat "$TEST_TARGET_FILE_STDOUT")";
  if [[ "$actual_output" != *"$expected_contains"* ]]; then
    _print_test_failure_loc;
    _assertion_failure "Program output (stdout) does not contain expected output";
    echo "Expected that output contains:";
    _print_expected "$expected_contains";
    echo "";
    echo "The full output is:";
    _print_actual "$actual_output";
    exit $EXIT_TEST_FAILURE;
  fi
  return $EXIT_SUCCESS;
}

# Asserts that the executable under test has not produced any output on stdout.
function assert_stdout_is_empty() {
  local actual_stdout_output="$(cat "$TEST_TARGET_FILE_STDOUT")";
  if [ -n "$actual_stdout_output" ]; then
    _print_test_failure_loc;
    _assertion_failure "Program output (stdout) is expected to be empty";
    echo "The actual output is:";
    _print_actual "$actual_stdout_output";
    exit $EXIT_TEST_FAILURE;
  fi
  return $EXIT_SUCCESS;
}

# Asserts that the executable under test has produced the specified output on stderr.
#
# Args:
# $* - The expected standard error output of the executable. This is a mandatory argument.
#
function assert_stderr_equals() {
  local IFS='';
  local expected_output="$*";
  IFS="${_IFS_RESET}";
  local actual_output="$(cat "$TEST_TARGET_FILE_STDERR")";
  if [[ "$actual_output" != "$expected_output" ]]; then
    _print_test_failure_loc;
    _assertion_failure "Program output (stderr) does not match expected output";
    _print_diff "$expected_output" "$actual_output";
    exit $EXIT_TEST_FAILURE;
  fi
  return $EXIT_SUCCESS;
}

# Asserts that the executable under test has produced output on stderr that
# contains the specified text.
#
# Args:
# $1 - The expected text that must be part of the executable's stderr output.
#      This is a mandatory argument.
#
function assert_stderr_contains() {
  local expected_contains="$1";
  local actual_output="$(cat "$TEST_TARGET_FILE_STDERR")";
  if [[ "$actual_output" != *"$expected_contains"* ]]; then
    _print_test_failure_loc;
    _assertion_failure "Program output (stderr) does not contain expected output";
    echo "Expected that output contains:";
    _print_expected "$expected_contains";
    echo "";
    echo "The full error output is:";
    _print_actual "$actual_output";
    exit $EXIT_TEST_FAILURE;
  fi
  return $EXIT_SUCCESS;
}

# Asserts that the executable under test has not produced any output on stderr.
function assert_stderr_is_empty() {
  local actual_stderr_output="$(cat "$TEST_TARGET_FILE_STDERR")";
  if [ -n "$actual_stderr_output" ]; then
    _print_test_failure_loc;
    _assertion_failure "Program output (stderr) is expected to be empty";
    echo "The actual error output is:";
    _print_actual "$actual_stderr_output";
    exit $EXIT_TEST_FAILURE;
  fi
  return $EXIT_SUCCESS;
}

# Main driver function.
#
# Args:
# $1 - The path to the test script to potentially launch. This is an optional argument.
#
function main() {
  local arg_test_script="$1";
  if [ -z "$arg_test_script" ]; then
    return $EXIT_SUCCESS;
  fi
  if [ -z "$TEST_TARGET_APP" ]; then
    echo "Error: Environment variable TEST_TARGET_APP is not set";
    return $EXIT_TEST_DRIVER_ERROR;
  fi
  if [ -z "$TEST_TARGET_NAME" ]; then
    echo "Error: Environment variable TEST_TARGET_NAME is not set";
    return $EXIT_TEST_DRIVER_ERROR;
  fi
  if [ -z "$TEST_PROJECT_DIR" ]; then
    echo "Error: Environment variable TEST_PROJECT_DIR is not set";
    return $EXIT_TEST_DRIVER_ERROR;
  fi
  if [ -n "$MSYSTEM" ]; then
    if command -v "cygpath" &> /dev/null; then
      # In an MSYS2 environment, convert path to mixed format
      # to match expectations of test assertions
      TEST_PROJECT_DIR="$(cygpath -m "$TEST_PROJECT_DIR")";
    fi
  fi
  export TEST_TARGET_DIR="${TEST_PROJECT_DIR}/build/Testing/output";
  if ! [ -d "$TEST_TARGET_DIR" ]; then
    if ! mkdir -p "$TEST_TARGET_DIR"; then
      echo "Error: Test directory not found and could not be created: '${TEST_TARGET_DIR}'";
      return $EXIT_TEST_DRIVER_ERROR;
    fi
  fi
  TEST_TARGET_FILE_STDOUT="${TEST_TARGET_DIR}/${TEST_TARGET_NAME}_stdout.txt";
  TEST_TARGET_FILE_STDERR="${TEST_TARGET_DIR}/${TEST_TARGET_NAME}_stderr.txt";
  if ! [ -f "$arg_test_script" ]; then
    echo "Error: Test script not found: '${arg_test_script}'";
    return $EXIT_TEST_DRIVER_ERROR;
  fi
  if ! source "$arg_test_script"; then
    echo "Error: Failed to source the specified test script: '${arg_test_script}'";
    return $EXIT_TEST_DRIVER_ERROR;
  fi
  local test_script_name="$(basename "$arg_test_script")";
  local func_name="";
  local func_srcfile="";
  local func_status=0;
  local func_found=false;
  for func_name in $(declare -F); do
    if [[ "$func_name" == *"test_"* ]]; then
      shopt -q -s extdebug;
      func_srcfile="$(declare -F ${func_name})";
      shopt -q -u extdebug;
      if [[ "$func_srcfile" == *"${arg_test_script}" ]]; then
        func_found=true;
        $func_name;
        func_status=$?;
        if (( func_status != 0 )); then
          echo "Test failure in function ${func_name}()";
          echo "Test function returned failure code ${func_status}";
          return $EXIT_TEST_FAILURE;
        fi
      fi
    fi
  done
  if [[ $func_found == false ]]; then
    echo "Error: No test function found in file '${arg_test_script}'";
    return $EXIT_TEST_DRIVER_ERROR;
  fi
  echo "Functionality tests in ${test_script_name} PASSED";
  return $EXIT_SUCCESS;
}

main "$@";
