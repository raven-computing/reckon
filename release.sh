#!/bin/bash
# Release script for the Reckon project.

USAGE="Usage: release.sh [options]";

HELP_TEXT=$(cat << EOS
Handles release versions of the Reckon project.

${USAGE}

Options:

  [--bump-version] <IDENT> Bump the project version, where IDENT is either 'M' for major,
                           'm' for minor or 'p' for patch. This option will result in the
                           project version being incremented at the corresponding level.

  [-?|--help]              Show this help message.
EOS
)

# Arg flags
ARG_BUMP_VERSION=false;
ARG_SHOW_HELP=false;

# Arg helper vars
arg_check_optarg=false;
arg_optarg_key="";
arg_optarg_required="";
arg_opt_arg="";

# Parse all arguments given to this script
for arg in "$@"; do
  if [[ $arg_check_optarg == true ]]; then
    arg_check_optarg=false;
    if [[ "$arg" != -* ]]; then
      arg_opt_arg="$arg";
      arg_optarg_required="";
      shift;
      continue;
    fi
  fi
  if [ -n "$arg_optarg_required" ]; then
    echo "Missing required option argument '${arg_optarg_required}'";
    exit 1;
  fi
  case $arg in
    --bump-version)
    ARG_BUMP_VERSION=true;
    arg_check_optarg=true;
    arg_optarg_required="IDENT";
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
    echo "Run 'release.sh --help' for more information";
    exit 1;
    ;;
  esac
done

# Check if help is triggered
if [[ $ARG_SHOW_HELP == true ]]; then
  echo "$HELP_TEXT";
  exit 0;
fi

if [ -n "$arg_optarg_required" ]; then
  echo "Missing required option argument '${arg_optarg_required}'";
  exit 1;
fi

if [[ "$(git branch --show-current)" != "master" ]]; then
  echo "Warning: Can only operate from within the 'master' branch. Aborting.";
  exit 1;
fi

git_current_status=$(git status --porcelain);
if [ -n "$git_current_status" ]; then
  echo "Warning: You have uncommitted or unstaged changes in your working tree.";
  echo "Please first commit or discard the following changes:";
  echo "$git_current_status";
  exit 1;
fi

if [[ $ARG_BUMP_VERSION == true ]]; then
  version_ident_index="";
  if [[ "$arg_opt_arg" == "M" ]]; then
    version_ident_index=0;
  elif [[ "$arg_opt_arg" == "m" ]]; then
    version_ident_index=1;
  elif [[ "$arg_opt_arg" == "p" ]]; then
    version_ident_index=2;
  else
    echo "Error: Argument for option '--bump-version' must be one of ('M', 'm', 'p')";
    exit 1;
  fi
  path_version_file="${PWD}/VERSION";
  VERSION="?.?.?";
  if [ -r "$path_version_file" ]; then
    VERSION=$(head -n 1 "$path_version_file");
    VERSION_BASE="${VERSION%%-*}";
  fi
  version_re="^[0-9]+\.[0-9]+\.[0-9]+(-dev)?$";
  if ! [[ $VERSION =~ $version_re ]]; then
    echo "Warning: Version specified in file 'VERSION' has an invalid format. Aborting.";
    exit 1;
  fi
  version_release="$VERSION";
  IS_DEV_VERSION=false;
  if [[ "$VERSION" == *-dev ]]; then
    IS_DEV_VERSION=true;
    version_release="${VERSION::-4}";
  fi
  version_array=( ${version_release//./ } );
  version_major="${version_array[0]}";
  version_minor="${version_array[1]}";
  version_patch="${version_array[2]}";
  version_new_value=$(( ${version_array[$version_ident_index]} + 1 ));
  version_array[$version_ident_index]="$version_new_value";
  new_version="${version_array[0]}.${version_array[1]}.${version_array[2]}";
  new_version_base="$new_version";
  if [[ $IS_DEV_VERSION == true ]]; then
    new_version="${new_version}-dev";
  fi
  echo "Bumping current version ${VERSION}";
  echo -n "$new_version" > "$path_version_file";
  sed -i -e "s/VERSION ${VERSION_BASE}/VERSION ${new_version_base}/" "CMakeLists.txt";
  sed -i -e "s/\x22${VERSION}\x22/\x22${new_version}\x22/" "docs/Doxyfile";
  echo "The new version is ${new_version}";
  echo "Committing version change";
  git add . && git commit -m "Bump version";
  if (( $? != 0 )); then
    echo "Error: Failed to commit changes";
    ecit 1;
  fi
  exit 0;
fi
