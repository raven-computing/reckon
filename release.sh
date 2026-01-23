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

git_current_status=$(git status --porcelain);
if [ -n "$git_current_status" ]; then
  echo "Warning: You have uncommitted or unstaged changes in your working tree.";
  echo "Please first commit or discard the following changes:";
  echo "$git_current_status";
  exit 1;
fi

readonly FILE_VERSION="VERSION";
readonly STAGING_BRANCH="staging";

VERSION="?.?.?";
VERSION_BASE="$VERSION";
IS_DEV_VERSION=false;
NEW_VERSION="";
NEW_VERSION_BASE="";

function read_version() {
  if [ -r "$FILE_VERSION" ]; then
    VERSION=$(head -n 1 "$FILE_VERSION");
    VERSION_BASE="${VERSION%%-*}";
  fi
  local version_re="^[0-9]+\.[0-9]+\.[0-9]+(-dev)?$";
  if ! [[ $VERSION =~ $version_re ]]; then
    echo "Warning: Version specified in file '${FILE_VERSION}' has an invalid format. Aborting.";
    exit 1;
  fi
  IS_DEV_VERSION=false;
  if [[ "$VERSION" == *-dev ]]; then
    IS_DEV_VERSION=true;
  fi
}

function update_version() {
  local version_change_index="$1";
  local version_array=( ${VERSION_BASE//./ } );
  local version_major="${version_array[0]}";
  local version_minor="${version_array[1]}";
  local version_patch="${version_array[2]}";
  if [ -n "$version_change_index" ]; then
    local version_new_value=$(( ${version_array[$version_change_index]} + 1 ));
    version_array[version_change_index]="$version_new_value";
    while (( version_change_index < 2 )); do
      ((++version_change_index));
      version_array[version_change_index]="0";
    done
  fi
  local new_version="${version_array[0]}.${version_array[1]}.${version_array[2]}";
  local new_version_base="$new_version";
  if [[ $IS_DEV_VERSION == true ]]; then
    new_version="${new_version}-dev";
  fi
  NEW_VERSION="$new_version";
  NEW_VERSION_BASE="$new_version_base";
}

function write_version() {
  echo -n "$NEW_VERSION" > "$FILE_VERSION";
  sed -i -e "s/VERSION ${VERSION_BASE}/VERSION ${NEW_VERSION_BASE}/" "CMakeLists.txt" || exit 1;
  sed -i -e "s/\x22${VERSION}\x22/\x22${NEW_VERSION}\x22/" "docs/Doxyfile" || exit 1;
}

function commit_release_candidate() {
  echo "Creating a release candidate";
  if git show-ref --verify --quiet "refs/heads/${STAGING_BRANCH}" \
    || git ls-remote --exit-code --heads origin "$STAGING_BRANCH" &> /dev/null; then

    echo "Warning: Branch '${STAGING_BRANCH}' already exists.";
    echo "A release candidate has already been prepared.";
    echo "You have to either cancel the ongoing release process by deleting the staging branch,";
    echo "or confirm the release by running this script from within the staging branch.";
    exit 1;
  fi
  if ! git checkout -b "${STAGING_BRANCH}"; then
    echo "Error: Failed to create '${STAGING_BRANCH}' branch";
    exit 1;
  fi
  read_version;
  IS_DEV_VERSION=false;
  update_version;
  echo "Setting version of release candidate to ${NEW_VERSION}";
  write_version;
  echo "Committing version change";
  if ! git commit -a -m "Release v${NEW_VERSION}-rc"; then
    echo "Error: Failed to commit changes";
    exit 1;
  fi
  echo "Pushing staging branch";
  if ! git push --set-upstream origin "${STAGING_BRANCH}"; then
    echo "Error: Failed to commit changes";
    exit 1;
  fi
  echo "";
  echo "The release candidate for v${NEW_VERSION} has been prepared.";
  echo "The current branch is '${STAGING_BRANCH}'.";
  echo "You can now perform manual checks and inspect automated builds and tests on GitHub.";
  echo "";
  echo "To CANCEL the release, delete the staging branch.";
  echo "To CONFIRM the release, run the release.sh again while on the staging branch.";
  echo "";
}

function commit_release() {
  echo "Committing release version";
  if ! git ls-remote --exit-code --heads origin "$STAGING_BRANCH" &> /dev/null; then
    echo "Error: No remote branch found for '${STAGING_BRANCH}'";
    exit 1;
  fi
  read_version;
  echo "Switching to master branch";
  if ! git checkout master; then
    echo "Error: Failed to switch to master branch";
    exit 1;
  fi
  echo "Syncing branch";
  if ! git pull; then
    echo "Error: Failed to pull from master branch";
    exit 1;
  fi
  local release_tag="v${VERSION}";
  if [[ "$(git tag -l ${release_tag})" != "" ]]; then
    echo "Error: A commit tag '${release_tag}' already exists";
    exit 1;
  fi
  echo "Merging staging branch into master";
  local commit_msg="Release v${VERSION}";
  if ! git merge -m "$commit_msg" --no-signoff "$STAGING_BRANCH"; then
    echo "Error: Failed to merge staging branch";
    exit 1;
  fi
  echo "Creating tag $release_tag";
  if ! git tag -s -m "$commit_msg" "$release_tag"; then
    echo "Error: Failed to tag release-version";
    exit 1;
  fi
  echo "Pushing release";
  git push && git push origin "$release_tag";
  if (( $? != 0 )); then
    echo "Failed to push release";
    exit 1;
  fi
}

function bump_version() {
  local version_ident_index="";
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
  read_version;
  update_version $version_ident_index;
  echo "Bumping current version ${VERSION}";
  write_version;
  echo "The new version is ${NEW_VERSION}";
  echo "Committing version change";
  git add . && git commit -m "Bump version" --no-signoff;
  if (( $? != 0 )); then
    echo "Error: Failed to commit changes";
    exit 1;
  fi
}

CURRENT_BRANCH="$(git branch --show-current)";

if [[ $ARG_BUMP_VERSION == true ]]; then
  bump_version;
  exit 0;
fi

if [[ "$CURRENT_BRANCH" == "master" ]]; then
  commit_release_candidate;
elif [[ "$CURRENT_BRANCH" == "staging" ]]; then
  commit_release;
else
  echo "Warning: Can only operate from within the 'master' or 'staging' branch. Aborting.";
  exit 1;
fi

exit 0;
