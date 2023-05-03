#!/usr/bin/env bash

# Set number of processes to use
HESTIA_LINTER_THREADS=4
if [ $HESTIA_BUILD_THREADS -ge $HESTIA_LINTER_THREADS ]
then
    HESTIA_LINTER_THREADS=$HESTIA_BUILD_THREADS
fi

# Exit on first error
set -o errexit


# Get the build directory from the command line
if [[ $# -ge 1 ]]
then
    # Get absolute path of build_dir from first argument.
    build_dir="$(cd "$1" && pwd)"
    shift
    echo
    echo "BUILD_DIRECTORY: ${build_dir}"
else
    echo
    echo "Error: wrong number of arguments"
    echo
    echo "usage: $(basename $0) BUILD_DIRECTORY"
    echo

    exit 1
fi

# Assume this file is in hestia/infra/scripts.
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo "SCRIPT DIRECTORY: ${script_dir}"
source_dir="$( cd "${script_dir}/../.." && pwd)"
echo "SOURCE DIRECTORY: ${source_dir}"
echo 


# All the directories containing source files
source_dirs="src test"

# Run lint.sh on every source file in hestia.
#
# Find all the .cc files in the project and run clang-tidy on them.
# Need to cd into source_dir because source_dirs are defined relative to that.
#
cd "${source_dir}"
find ${source_dirs} \( \
    -iname "*.cc" -o -iname "*.c" \
\) -print0 \
    |  xargs -0 -P "${HESTIA_LINTER_THREADS}" -I TARGET_FILE "${script_dir}/lint.sh" "${build_dir}" TARGET_FILE "$@"
