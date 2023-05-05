#!/usr/bin/env bash

# Exit on first error
set -o errexit

HESTIA_FORMATTER_THREADS=4
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    HESTIA_FORMATTER_THREADS=$(nproc)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    HESTIA_FORMATTER_THREADS=$(sysctl -n hw.logicalcpu)
fi

# Assume current script is in hestia/tools 
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo "SCRIPT DIRECTORY: ${script_dir}"
source_dir="$( cd "${script_dir}/../.." && pwd)"
echo "SOURCE DIRECTORY: ${source_dir}"
echo 

# Get the format script
FORMAT_EXECUTABLE="${script_dir}/format.sh"

cd ${source_dir}

# All the directories containing source files
source_dirs="src test"

# All the extensions of source files
source_exts="cc hh h"

# Number of processes to run in parallel
nprocs=$HESTIA_FORMATTER_THREADS

for dir in $source_dirs
do
    for ext in $source_exts
    do
        # Run clang-format on all the files with the given extension in each
        # directory
        if [ ! -z "$(find $dir -iname "*.${ext}")" ]
        then
            find $dir -iname "*.${ext}" -print0 \
                | xargs -n 1 -0 -P "${nprocs}" ${FORMAT_EXECUTABLE}
        fi
    done
    #if [ ! -z "$(find $dir -iname "*.py")" ]
    #then
    #    yapf -ir $dir
    #fi
done

