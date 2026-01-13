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


function test_help_argument_prints_usage_text() {
  run_app --help;
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_contains "Usage: scount ";
  assert_stdout_contains "Positional Arguments:";
  assert_stdout_contains "Options:";
  assert_stderr_is_empty;
}

function test_version_long_argument_prints_version_text() {
  local project_version="$(cat "${TEST_PROJECT_DIR}/VERSION")";
  run_app --version;
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_contains "scount v${project_version}";
  assert_stdout_contains "Copyright";
  assert_stdout_contains "This software is licensed under";
  assert_stderr_is_empty;
}

function test_version_short_argument_prints_only_version_info() {
  local project_version="$(cat "${TEST_PROJECT_DIR}/VERSION")";
  run_app -#;
  assert_exit_status $EXIT_SUCCESS;
  assert_stdout_equals "${project_version}${_NL}";
  assert_stderr_is_empty;
}

function test_exit_status_when_std_streams_are_closed() {
  local arg="${TEST_PROJECT_DIR}/src/lib/tests/res/java/Sample.java";
  "$TEST_TARGET_APP" "$arg" >&- 2>&-;
  TEST_TARGET_APP_EXIT_STATUS=$?;
  assert_exit_status $EXIT_PROG_IO_ERROR;
}
