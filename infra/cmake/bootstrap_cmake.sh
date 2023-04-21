#!/usr/bin/env bash

ARCH=$(arch)
CMAKE_VERSION=3.26.3
CMAKE_ARCHIVE_NAME=cmake-$CMAKE_VERSION-linux-$ARCH

wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/$CMAKE_ARCHIVE_NAME.tar.gz --no-check-certificate
tar -xvf $CMAKE_ARCHIVE_NAME.tar.gz 
ln -s $CMAKE_ARCHIVE_NAME/bin/cmake cmake

