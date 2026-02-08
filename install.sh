#!/bin/bash
# Install script for the Reckon project.

USAGE="Usage: install.sh [options]";

HELP_TEXT=$(cat << EOS
Installs the scount executable.

If a build is not available, a release variant will be automatically built from source.
If this install script is run with superuser privileges, then the target will be
installed in a system-wide installation directory. Otherwise, when it is run with
regular user privileges, installation will be done in the user's local directory.

${USAGE}

Options:

  [--yes]     Skip the confirmation prompt.

  [-?|--help] Show this help message.
EOS
)

# Arg flags
ARG_SHOW_HELP=false;
ARG_ASSUME_YES=false;

# Parse all arguments given to this script
for arg in "$@"; do
  case $arg in
    --yes)
    ARG_ASSUME_YES=true;
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
    echo "Run 'install.sh --help' for more information";
    exit 1;
    ;;
  esac
done

# Check if help is triggered
if [[ $ARG_SHOW_HELP == true ]]; then
  echo "$HELP_TEXT";
  exit 0;
fi

# Check if the build dir is empty
if [ -z "$(ls -A build)" ]; then
  echo "Building project before installation";
  if ! bash build.sh; then
    exit $?;
  fi
fi

BUILD_CONFIGURATION="";
# Determine the build configuration of the last build.
if [ -f "build/CMakeCache.txt" ]; then
  BUILD_CONFIGURATION="$(grep --max-count=1 CMAKE_BUILD_TYPE build/CMakeCache.txt \
                         | cut  --delimiter='=' --fields=2)";
fi
if [ -z "$BUILD_CONFIGURATION" ]; then
  if [ -d "build/bin/Release" ]; then
    BUILD_CONFIGURATION="Release";
  elif [ -d "build/bin/Debug" ]; then
    BUILD_CONFIGURATION="Debug";
  fi
fi
if [ -z "$BUILD_CONFIGURATION" ]; then
  echo "Error: No build configuration found.";
  exit 1;
fi

INSTALL_PREFIX="/usr/local";
if (( $(id -u) != 0 )); then
  INSTALL_PREFIX="${HOME}/.local";
fi
if [ -n "$MSYSTEM" ]; then
  INSTALL_PREFIX="${HOME}/AppData/Local/Programs";
fi

if [[ $ARG_ASSUME_YES == false ]]; then
  if [[ ! -t 0 ]]; then
    echo "Error: Refusing to install without confirmation in a non-interactive shell.";
    echo "Re-run with '--yes' to proceed.";
    exit 1;
  fi
  read -r -p "Do you want to install scount? (y/N): " ANSWER;
  case "$ANSWER" in
    1|YES|yes|Yes|Y|y|true|True)
    ;;
    0|NO|no|No|N|n|false|False)
    echo "Abort.";
    exit 1;
    ;;
    *)
    echo "Invalid input. Please enter either yes or no";
    exit 1;
    ;;
  esac
fi

echo "Installing scount application";

if ! cmake --install build --component "reckon_scount" --prefix "$INSTALL_PREFIX"; then
  exit $?;
fi
echo "Installation complete";
exit 0;
