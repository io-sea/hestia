#!/bin/bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source_dir="$script_dir/../.."

cd $source_dir
mkdir -p build
build_dir=$source_dir/build
cd $build_dir 
MOTR_SRC=$build_dir/cortx-motr

git clone --recursive https://github.com/Seagate/cortx-motr.git $MOTR_SRC
#set version of motr to match the version from the rpm, by default this is the latest version
#MOTR_SHA=
#pushd $MOTR_SRC && git reset --hard $MOTR_SHA && popd

cmake -DHESTIA_WITH_MOTR=ON -DMOTR_SRC_DIR=$MOTR_SRC ..
make -j 4


