#!/bin/bash
# Test script for the Reckon project.

USAGE="Usage: test.sh [options]";

HELP_TEXT=$(cat << EOS
Tests the Reckon library and scount application.

${USAGE}

Options:

  [--all]              Execute the entire test suite and all automated checks.

  [--check]            Execute various consistency checks.

  [--coverage]         Report code coverage metrics for the test runs.
                       Targets must have been built with debug symbols.

  [-f|--functionality] Execute functionality tests.

  [-i|--integration]   Execute integration tests.

  [-l|--lint]          Perform static code analysis with a linter.

  [--stop-on-failure]  Stop the test execution immediately after the
                       first failure is encountered.

  [-u|--unit]          Execute unit tests.

  [-?|--help]          Show this help message.
EOS
)

# Arg flags
ARG_ALL=false;
ARG_CHECK=false;
ARG_COVERAGE=false;
ARG_FUNCTIONALITY=false;
ARG_INTEGRATION=false;
ARG_LINT=false;
ARG_STOP_ON_FAILURE=false;
ARG_UNIT=false;
ARG_SHOW_HELP=false;

# Parse all arguments given to this script
for arg in "$@"; do
  case $arg in
    --all)
    ARG_ALL=true;
    ARG_CHECK=true;
    ARG_LINT=true;
    shift
    ;;
    --check)
    ARG_CHECK=true;
    shift
    ;;
    --coverage)
    ARG_COVERAGE=true;
    shift
    ;;
    -f|--functionality)
    ARG_FUNCTIONALITY=true;
    shift
    ;;
    -i|--integration)
    ARG_INTEGRATION=true;
    shift
    ;;
    -l|--lint)
    ARG_LINT=true;
    shift
    ;;
    --stop-on-failure)
    ARG_STOP_ON_FAILURE=true;
    shift
    ;;
    -u|--unit)
    ARG_UNIT=true;
    shift
    ;;
    -\?|--help)
    ARG_SHOW_HELP=true;
    shift
    ;;
    *)
    # Unknown Argument
    echo "Unknown argument: '$arg'";
    echo "$USAGE";
    echo "";
    echo "Run 'test.sh --help' for more information";
    exit 1;
    ;;
  esac
done

# Check if help is triggered
if [[ $ARG_SHOW_HELP == true ]]; then
  echo "$HELP_TEXT";
  exit 0;
fi

# Need to prematurely create the build dir if it does not exist
# so it can be used as a volume by Docker when using
# an isolated test execution
if ! [ -d "build" ]; then
  mkdir "build";
fi

has_apt=false;
if command -v "apt" &> /dev/null; then
  has_apt=true;
fi

# Ensure the required executables are available
if ! command -v "cmake" &> /dev/null; then
  echo "ERROR: Could not find the 'cmake' executable.";
  echo "ERROR: Please make sure that CMake is correctly installed";
  if [[ $has_apt == true ]]; then
    echo "Perhaps you can install it with 'sudo apt install cmake'";
  fi
  exit 1;
fi
if ! command -v "ctest" &> /dev/null; then
  echo "ERROR: Could not find the 'ctest' executable.";
  echo "ERROR: Please make sure that CMake and CTest are correctly installed";
  exit 1;
fi
if [[ $ARG_COVERAGE == true ]]; then
  if ! command -v "lcov" &> /dev/null; then
    echo "ERROR: Could not find command 'lcov' to generate test coverage data";
    if [[ $has_apt == true ]]; then
      echo "Perhaps you can install it with 'sudo apt install lcov'";
    fi
    exit 1;
  fi
  if ! command -v "genhtml" &> /dev/null; then
    echo "ERROR: Could not find command 'genhtml' to generate test coverage report";
    exit 1;
  fi
fi

# Check if the build dir is empty
if [ -z "$(ls -A build)" ]; then
  # Build the tests first
  build_args=();
  if [[ $ARG_COVERAGE == true ]]; then
    build_args+=("--debug");
    build_args+=("--coverage");
  fi
  echo "Building project before test run execution";
  if ! bash build.sh "${build_args[@]}"; then
    exit $?;
  fi
fi

if [[ $ARG_CHECK == true ]]; then
  echo "Performing source code checks";
  check_failed=false;
  check_found_files=false;
  while IFS= read -r file; do
    check_found_files=true;
    pattern='^[[:space:]]*void[[:space:]]+test[[:alnum:]_]*'\
'[[:space:]]*\([[:space:]]*void[[:space:]]*\)';
    defs="$(grep -Eo "$pattern" "$file" |wc -l)";
    pattern='RUN_TEST[[:space:]]*\([[:space:]]*test[[:alnum:]'\
'_]*[[:space:]]*\)[[:space:]]*;';
    runs="$(grep -Eo "$pattern" "$file" |wc -l)";
    if [[ "$defs" != "$runs" ]]; then
      echo -n "Error: Test consistency: ";
      echo -n "'${file}' defines ${defs} test function(s) ";
      echo "but has ${runs} RUN_TEST() usage(s)";
      check_failed=true;
    fi
  done < <(find . -type f -name 'test_*.c' -path '*/tests/*');

  if [[ $check_found_files == false ]]; then
    echo "Note: C test consistency: no matching files found (*/tests/*/test_*.c)";
  fi
  if [[ $check_failed == true ]]; then
    echo "Did you forget to register test functions via the RUN_TEST() macro?";
    exit 1;
  fi
  echo "Consistency of test code: OK";
  if [[ $ARG_ALL == false ]]; then
    exit 0;
  fi
fi

if [[ $ARG_LINT == true ]]; then
  if ! command -v "run-clang-tidy" &> /dev/null; then
    echo "ERROR: Could not find command 'run-clang-tidy'";
    if [[ $has_apt == true ]]; then
      echo "Perhaps you can install it with 'sudo apt install clang-tidy'";
    fi
    exit 1;
  fi
  echo "Performing static code analysis";
  lint_out=$(run-clang-tidy -quiet -config '' -p build "${PWD}/src" 2> /dev/null);
  tidy_stat=$?;
  if (( tidy_stat != 0 )); then
    echo "Static code analysis found the following issues:";
    lint_out="$(echo "$lint_out" |grep --invert-match '^clang-tidy-')";
    echo "$lint_out";
    exit $tidy_stat;
  else
    echo "No issues found in source files";
  fi
  if [[ $ARG_ALL == false ]]; then
    exit $tidy_stat;
  fi
fi

COV_INCL_PATH="$PWD";
BUILD_DIR_COV_DATA="cov";
FILE_COV_DATA_MERGED="merged.info";
BUILD_DIR_COV_REPORT="coverage_report";

VERSION="$(cat VERSION)";
if [[ "$VERSION" == *"-dev" ]]; then
  VERSION+=" (Development Version)";
fi

cd "build" || exit 1;

if [[ $ARG_COVERAGE == true ]]; then
  if ! cmake --build . --target clean_coverage_data 1>/dev/null; then
    echo "Warning: Failed to clean existing coverage data files";
  fi
fi

BUILD_CONFIGURATION="";
# Determine the build configuration of the last build.
if [ -f "CMakeCache.txt" ]; then
  BUILD_CONFIGURATION="$(grep --max-count=1 CMAKE_BUILD_TYPE CMakeCache.txt \
                         | cut  --delimiter='=' --fields=2)";
fi
if [ -z "$BUILD_CONFIGURATION" ]; then
  if [ -d "bin/Debug" ]; then
    BUILD_CONFIGURATION="Debug";
  fi
  if [ -d "bin/Release" ]; then
    if [[ "$BUILD_CONFIGURATION" == "Debug" ]]; then
      echo "Warning: Multiple build configurations found. Using 'Release' for testing.";
    fi
    BUILD_CONFIGURATION="Release";
  fi
fi
if [ -z "$BUILD_CONFIGURATION" ]; then
  echo "Error: No build configuration found.";
  exit 1;
fi

CTEST_LABEL_OPT="";
CTEST_LABEL_OPT_ARG="(";
if [[ $ARG_UNIT == true ]]; then
  CTEST_LABEL_OPT_ARG+="Unit";
fi
if [[ $ARG_INTEGRATION == true ]]; then
  if [[ "$CTEST_LABEL_OPT_ARG" != "(" ]]; then
    CTEST_LABEL_OPT_ARG+="|";
  fi
  CTEST_LABEL_OPT_ARG+="Integration";
fi
if [[ $ARG_FUNCTIONALITY == true ]]; then
  if [[ "$CTEST_LABEL_OPT_ARG" != "(" ]]; then
    CTEST_LABEL_OPT_ARG+="|";
  fi
  CTEST_LABEL_OPT_ARG+="Functionality";
fi
if [[ "$CTEST_LABEL_OPT_ARG" != "(" ]]; then
  CTEST_LABEL_OPT="--label-regex";
  CTEST_LABEL_OPT_ARG+=")";
else
  CTEST_LABEL_OPT_ARG="";
fi

CTEST_ARG_STOP_ON_FAILURE="";
if [[ $ARG_STOP_ON_FAILURE == true ]]; then
  CTEST_ARG_STOP_ON_FAILURE="--stop-on-failure";
fi

# UB-Sanitizer options.
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1";

# Run tests with CTest
ctest --output-on-failure \
      --build-config "$BUILD_CONFIGURATION" \
      $CTEST_ARG_STOP_ON_FAILURE \
      $CTEST_LABEL_OPT $CTEST_LABEL_OPT_ARG;

ctest_status=$?;

if (( ctest_status == 0 )); then
  if [[ $ARG_COVERAGE == true ]]; then
    echo "Collecting coverage data";
    if ! cmake --build . --target collect_coverage_data 1>/dev/null; then
      echo "Failed to collect test coverage data";
      exit 1;
    fi
    echo "Generating test coverage report";
    genhtml_opt_args=();
    genhtml_version=($(genhtml --version |grep -o -e '[0-9.]'));
    genhtml_version="${genhtml_version[0]}";
    if (( genhtml_version >= 2 )); then
      genhtml_opt_args+=("--header-title");
      genhtml_opt_args+=("Reckon Test Coverage");
      genhtml_opt_args+=("--footer");
      genhtml_opt_args+=("Reckon - ${VERSION}");
    fi
    if ! genhtml --quiet --output-directory "$BUILD_DIR_COV_REPORT" \
                 --prefix "$COV_INCL_PATH" \
                 --title "Reckon Test Coverage" \
                 "${genhtml_opt_args[@]}" \
                 "${BUILD_DIR_COV_DATA}/${FILE_COV_DATA_MERGED}"; then
      echo "Failed to generate test coverage report";
      exit 1;
    fi
    report_label="build/${BUILD_DIR_COV_REPORT}/index.html";
    report_url="file://${COV_INCL_PATH}/${report_label}";
    report_link="\e]8;;${report_url}\e\\\\${report_label}\e]8;;\e\\\\";
    echo -e "Wrote HTML report to $report_link";
  fi
fi

exit $ctest_status;
