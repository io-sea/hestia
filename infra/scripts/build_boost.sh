#!/usr/bin/env bash

wget https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz
tar -xvf boost_1_76_0.tar.gz
cd boost_1_76_0
./bootstrap.sh --prefix=/opt/boost;
./b2 install --prefix=/opt/boost --with=all