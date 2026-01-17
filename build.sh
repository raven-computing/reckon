#!/bin/bash
# Build script for the Reckon project.

USAGE="Usage: build.sh [options]";

HELP_TEXT=$(cat << EOS
Builds the Reckon library and scount application.

${USAGE}

Options:

  [--analyze]     Enable static source code analysis checks by the compiler.
                  This can slow down compilation time significantly.

  [--clean]       Remove all build-related directories and files and then exit.

  [--config]      Only execute the build configuration step. This option will skip
                  the build step.

  [--coverage]    Enable code coverage instrumentation. Should only be used with debug builds.

  [--debug]       Build with debug symbols and with optimizations turned off.

  [--docs]        Build the documentation and then exit.

  [--ignore-warnings]
                  Ignore all compiler warnings during the build process. Warning messages
                  may still be shown, but will not cause the build to fail.

  [--only-libs]   Only build the libraries.

  [--package]     Build a native installable package. The scount target must already be built.

  [--sanitizers]  Use sanitizers when building and running.

  [--shared]      Build shared libraries instead of static libraries.

  [--skip-config] Skip the build configuration step. If the build tree does not
                  exist yet, then this option has no effect and the build
                  configuration step is executed.

  [--skip-tests]  Do not build any tests.

  [-?|--help]     Show this help message.
EOS
)

# Arg flags
ARG_ANALYZE=false;
ARG_CLEAN=false;
ARG_CONFIG=false;
ARG_COVERAGE=false;
ARG_PACKAGE=false;
ARG_SANITIZERS=false;
ARG_SHARED=false;
ARG_DEBUG=false;
ARG_DOCS=false;
ARG_IGNORE_WARNINGS=false;
ARG_ONLY_LIBS=false;
ARG_SKIP_CONFIG=false;
ARG_SKIP_TESTS=false;
ARG_SHOW_HELP=false;

# Parse all arguments given to this script
for arg in "$@"; do
  case $arg in
    --analyze)
    ARG_ANALYZE=true;
    shift
    ;;
    --clean)
    ARG_CLEAN=true;
    shift
    ;;
    --config)
    ARG_CONFIG=true;
    shift
    ;;
    --coverage)
    ARG_COVERAGE=true;
    shift
    ;;
    --package)
    ARG_PACKAGE=true;
    shift
    ;;
    --sanitizers)
    ARG_SANITIZERS=true;
    shift
    ;;
    --shared)
    ARG_SHARED=true;
    shift
    ;;
    --debug)
    ARG_DEBUG=true;
    shift
    ;;
    --ignore-warnings)
    ARG_IGNORE_WARNINGS=true;
    shift
    ;;
    --only-libs)
    ARG_ONLY_LIBS=true;
    shift
    ;;
    --docs)
    ARG_DOCS=true;
    shift
    ;;
    --skip-config)
    ARG_SKIP_CONFIG=true;
    shift
    ;;
    --skip-tests)
    ARG_SKIP_TESTS=true;
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
    echo "Run 'build.sh --help' for more information";
    exit 1;
    ;;
  esac
done

# Check if help is triggered
if [[ $ARG_SHOW_HELP == true ]]; then
  echo "$HELP_TEXT";
  exit 0;
fi

if [[ $ARG_CONFIG == true && $ARG_SKIP_CONFIG == true ]]; then
  echo "Cannot specify both --config and --skip-config options";
  exit 1;
fi

# Check clean flag
if [[ $ARG_CLEAN == true ]]; then
  if [ -d "build" ]; then
    rm -rf "build";
  fi
  exit 0;
fi

if ! [ -d "build" ]; then
  mkdir "build";
fi


if [[ $ARG_DOCS == true ]]; then
  if ! command -v "doxygen" &> /dev/null; then
    echo "Could not find the 'doxygen' executable.";
    echo "Please make sure that Doxygen is correctly installed";
    exit 1;
  fi
  echo "Configuring documentation";
  if ! cmake -P "cmake/DocsUtil.cmake"; then
    echo "CMake Error: Documentation configure step has failed";
    exit 1;
  fi
  echo "Building documentation";
  export DOXYGEN_THEME_PATH="build/_deps/doxygen-theme";
  doxygen "docs/Doxyfile" &> /dev/null;
  docs_status=$?;
  if (( docs_status != 0 )); then
    echo "Failed to build documentation";
    echo "Doxygen finished with exit status $docs_status";
  fi
  page_label="build/docs/html/index.html";
  page_url="file://${PWD}/${page_label}";
  page_link="\e]8;;${page_url}\e\\\\${page_label}\e]8;;\e\\\\";
  echo -e "Documentation HTML page is available at ${page_link}";
  exit $docs_status;
fi

# Ensure the required executable is available
if ! command -v "cmake" &> /dev/null; then
  echo "ERROR: Could not find the 'cmake' executable.";
  echo "Please make sure that CMake is correctly installed";
  exit 1;
fi

if command -v "gzip" &> /dev/null; then
  # Compress the scount man page; this is used for packaging
  mkdir -p "build/man";
  if ! gzip -9n --stdout "man/scount.1" > "build/man/scount.1.gz"; then
    echo "Warning: Failed to compress man page 'man/scount.1'";
  fi
else
  echo "Warning: Could not find the 'gzip' executable.";
  echo "Could not compress man page";
fi

cd "build";

BUILD_CONFIGURATION="Release";

if [[ $ARG_DEBUG == true ]]; then
  BUILD_CONFIGURATION="Debug";
fi

BUILD_TESTS="ON";
IGNORE_WARNINGS="OFF";
BUILD_ONLY_LIBS=OFF;
BUILD_ANALYZE="OFF";
BUILD_WITH_SANITIZERS="OFF";
BUILD_WITH_COVERAGE="OFF";

if [[ $ARG_IGNORE_WARNINGS == true ]]; then
  IGNORE_WARNINGS="ON";
fi
if [[ $ARG_ONLY_LIBS == true ]]; then
  BUILD_ONLY_LIBS=ON;
fi
if [[ $ARG_ANALYZE == true ]]; then
  BUILD_ANALYZE="ON";
fi
if [[ $ARG_SKIP_TESTS == true ]]; then
  BUILD_TESTS="OFF";
fi
if [[ $ARG_SANITIZERS == true ]]; then
  BUILD_WITH_SANITIZERS="ON";
fi

BUILD_SHARED_LIBS="OFF";

if [[ $ARG_SHARED == true ]]; then
  BUILD_SHARED_LIBS="ON";
fi
if [[ $ARG_COVERAGE == true ]]; then
  BUILD_WITH_COVERAGE="ON";
  if [[ $ARG_SKIP_TESTS == true ]]; then
    echo "Warning: Unable to automatically generate test coverage reports" \
         "when test builds are skipped";
  fi
fi

if [[ $ARG_PACKAGE == true ]]; then
  # CMake: Package
  cmake --build . --target package --config "$BUILD_CONFIGURATION";
  exit $?;
fi

# CMake: Configure
if [[ $ARG_SKIP_CONFIG == false ]]; then
  cmake -DCMAKE_BUILD_TYPE="$BUILD_CONFIGURATION" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DRECKON_IGNORE_WARNINGS="$IGNORE_WARNINGS" \
        -DRECKON_SOURCE_ANALYSIS="$BUILD_ANALYZE" \
        -DRECKON_BUILD_TESTS="$BUILD_TESTS" \
        -DRECKON_BUILD_SHARED_LIBS="$BUILD_SHARED_LIBS" \
        -DRECKON_BUILD_ONLY_LIBS="$BUILD_ONLY_LIBS" \
        -DRECKON_USE_SANITIZERS="$BUILD_WITH_SANITIZERS" \
        -DRECKON_BUILD_TEST_COVERAGE="$BUILD_WITH_COVERAGE" ..;

  config_status=$?;
  if (( config_status != 0 )); then
    exit $config_status;
  fi
  if [[ $ARG_CONFIG == true ]]; then
    exit $config_status;
  fi
fi

# CMake: Build
cmake --build . --config $BUILD_CONFIGURATION --parallel $(nproc);
exit $?;
