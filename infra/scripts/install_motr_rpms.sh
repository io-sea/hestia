#!/usr/bin/env bash

#install hestia base dependencies before running this script

export MOTR_BUILDS_LOCATION=$1
if [ ! -d $1 ]; then
        echo "Error, pass in the path to the motr rpms folder"
        exit 1
fi

export MOTR_RPM_DIR=${MOTR_BUILDS_LOCATION}/builds/io-sea-internal/hestia/motr-rpms

yum update -y
yum install -y epel-release
yum config-manager --set-enabled powertools
yum install -y $(cat ../deps/rocky8/base)

if [ ! -d $MOTR_RPM_DIR ]; then
    echo "Error, pass in the top level motr rpms folder"
    exit 1
fi 
export HOST_ARCH=$(arch)

yum install -qy ${MOTR_RPM_DIR}/isa-l-2.30.0*.$(arch).rpm;
yum install -qy ${MOTR_RPM_DIR}/cortx-motr-2.0.0*.$(arch).rpm;
yum install -qy ${MOTR_RPM_DIR}/cortx-motr-devel-2.0.0*.$(arch).rpm;


python3 -m pip install --upgrade pip
python3 -m pip install --no-cache-dir asciidoc ply numpy==1.19.5
python3 -m pip install --no-cache-dir  cffi==1.14.5

if [ $HOST_ARCH == "aarch64" ];then
    # On aarch64 nothing gives kafka python wheel and repo lib is too old, so need to build.
    cd /;curl -Ls https://github.com/confluentinc/librdkafka/archive/refs/tags/v1.5.0.tar.gz | tar xz;cd librdkafka-1.5.0;
    ./configure --install-deps;make -j$(nproc);make install
    python3 -m pip install confluent-kafka==1.5.0
fi 

wget https://raw.githubusercontent.com/Seagate/cortx-utils/main/py-utils/python_requirements.txt
python3 -m pip install -r python_requirements.txt
python3 -m pip install --ignore-installed PyYAML==5.4.1

yum config-manager --add-repo https://rpm.releases.hashicorp.com/RHEL/hashicorp.repo # Provides consul dependency

yum install -y ${MOTR_RPM_DIR}/cortx-py-utils-2.0.0*.noarch.rpm 
yum install -y ${MOTR_RPM_DIR}/cortx-hare-2.0.0*.$(arch).rpm

interface=$(ifconfig | grep -o e.*:[[space]] | cut -d : -f 1)
echo "tcp("$interface")" > /etc/libfab.conf
