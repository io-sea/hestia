#!/usr/bin/env bash

git clone https://github.com/cea-hpc/phobos.git
cd phobos
patch -p1 --forward < ../phobos_ldm_common_page_size.patch
patch -p1 --forward < ../phobos_fix_db_conn.patch
./autogen.sh
CFLAGS=-Wno-error ./configure --disable-dependency-tracking
make
make install