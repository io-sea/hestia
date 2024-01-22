#!/bin/sh
rm -rf $1
mkdir -p $1
# chmod 777 -R $1 # for debug as non-root

export PYTHONPATH=/phobos/src/cli/build/lib.linux-$(arch)-3.6
phobos=/phobos/src/cli/build/scripts-3.6/phobos
$phobos dir add $1
$phobos dir format --fs posix --unlock $1
$phobos dir update --tags $2 $1
