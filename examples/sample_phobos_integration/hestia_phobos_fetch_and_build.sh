#!/usr/bin/env bash

chmod 755 build_phobos.sh init_phobos_db.sh start_phobos.sh phobos_add_dir.sh
./build_phobos
rm -rf /phobos

export HESTIA_VERSION="1.2.4"
wget https://git.ichec.ie/io-sea-internal/hestia/-/archive/v$HESTIA_VERSION/hestia-v$HESTIA_VERSION.tar.gz
tar xvf hestia-v$HESTIA_VERSION.tar.gz
rm hestia-v$HESTIA_VERSION.tar.gz
mkdir build
cd build
cmake -DHESTIA_WITH_PHOBOS=ON ../hestia-v$HESTIA_VERSION
make -j 8
make install
cd ..
rm -rf build
rm -rf hestia-v$HESTIA_VERSION

pip3 install --no-cache-dir -r phobos_requirements.txt
./init_phobos_db
