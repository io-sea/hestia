#!/usr/bin/env bash

patch_dir="$1"

git clone https://github.com/cea-hpc/phobos.git
cd phobos
patch -p1 --forward < $patch_dir/phobos_ldm_common_page_size.patch
./autogen.sh
CFLAGS=-Wno-error ./configure --disable-dependency-tracking
make
make install