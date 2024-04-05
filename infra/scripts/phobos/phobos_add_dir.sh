#!/bin/sh
rm -rf $1
mkdir -p $1

phobos dir add $1
phobos dir format --fs posix --unlock $1
phobos dir update --tags $2 $1
