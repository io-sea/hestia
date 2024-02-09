#!/usr/bin/env bash
export HOST_ARCH=$(arch)
export HOST_RELEASE=el8

export RPM_CACHE=/root/rpmbuild/RPMS/$HOST_ARCH
mkdir -p $MOTR_BUILD_DIR
mkdir -p $MOTR_RPM_DIR

# Get Motr sources
export MOTR_VERSION=2.0.0-1_git71735b89
export MOTR_SHA=71735b89b26fec78cea0f5fb43dadd964486fff5
export MOTR_SRC=$MOTR_BUILD_DIR/cortx-motr
git clone --recursive https://github.com/Seagate/cortx-motr.git $MOTR_SRC
pushd $MOTR_SRC && git reset --hard $MOTR_SHA && popd

export CORTX_UTILS_VERSION=2.0.0-2_95e10fa
export CORTX_UTILS_SHA=95e10fa204bd8f4d38aa7c2634e764022475342e
export CORTX_UTILS_SRC=$MOTR_BUILD_DIR/cortx-utils
git clone https://github.com/Seagate/cortx-utils.git $CORTX_UTILS_SRC
pushd $CORTX_UTILS_SRC && git reset --hard $CORTX_UTILS_SHA && popd

if [ $HOST_ARCH != "aarch64" ];then
    export HARE_VERSION=2.0.0-1_git6915b56
    export HARE_SHA=6915b5670f12c666900261dfbef56b8e719054a0
    export HARE_SRC=$MOTR_BUILD_DIR/cortx-hare
    git clone https://github.com/Seagate/cortx-hare.git $HARE_SRC
    pushd $HARE_SRC && git reset --hard $HARE_SHA && popd
fi

# Install dependency ISA_L 
export ISA_L_VERSION=2.30.0
rpmbuild --define '_isa_l_version 2.30.0' --bb $MOTR_SRC/scripts/provisioning/roles/motr-build/tasks/isa-l.spec
yum install -y $RPM_CACHE/isa-l-2.30.0-1.$HOST_RELEASE.$HOST_ARCH.rpm

# Add comment line to test rpm generation
# Make and install cortx-motr
cd $MOTR_SRC
./autogen.sh
./configure --prefix=$MOTR_BUILD_DIR --with-user-mode-only
make -j $(nproc)
export PYTHONPATH=$PYTHONPATH:/usr/local/lib/python3.6/site-packages/
make install -j $(nproc)
make rpms -j $(nproc)

yum install -y $RPM_CACHE/cortx-motr-$MOTR_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm
yum install -y $RPM_CACHE/cortx-motr-devel-$MOTR_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm

# Make and install cortx-py-utils
if [ $HOST_ARCH == "aarch64" ];then
    # On aarch64 nothing gives kafka python wheel and repo lib is too old, so need to build.
    cd /;curl -Ls https://github.com/confluentinc/librdkafka/archive/refs/tags/v1.5.0.tar.gz | tar xz;cd librdkafka-1.5.0;
    ./configure --install-deps;make -j$(nproc);make install
    python3 -m pip install confluent-kafka==1.5.0
fi 

cd $CORTX_UTILS_SRC
$CORTX_UTILS_SRC/jenkins/build.sh -v 2.0.0 -b 2
pip3 install -r $CORTX_UTILS_SRC/py-utils/python_requirements.txt
yum install -y $CORTX_UTILS_SRC/py-utils/dist/cortx-py-utils-$CORTX_UTILS_VERSION.noarch.rpm

# Make and install cortx-hare
if [ $HOST_ARCH != "aarch64" ];then
    # On aarch64 this fails as the python wheel name is hard coded to x86_64 in hax
    cd $HARE_SRC
    make rpm -j$(nproc)
    yum-config-manager --add-repo https://rpm.releases.hashicorp.com/RHEL/hashicorp.repo # Provides consul dependency to test rpm
    yum install -y $RPM_CACHE/cortx-hare-$HARE_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm
    cp -f $RPM_CACHE/cortx-hare-*.rpm $MOTR_RPM_DIR
fi

# Copy generated RPMs to output directory
cp -f $RPM_CACHE/isa-l-*.rpm $MOTR_RPM_DIR
cp -f $RPM_CACHE/cortx-*.rpm $MOTR_RPM_DIR
cp -f $CORTX_UTILS_SRC/py-utils/dist/cortx-py-utils-$CORTX_UTILS_VERSION.noarch.rpm $MOTR_RPM_DIR

# libmotr-ut missing from cortx-motr rpm and rpm-test target is broken
mkdir -p $MOTR_RPM_DIR/libmotr-ut/
cp -f $MOTR_BUILD_DIR/lib/libmotr-ut.so* $MOTR_RPM_DIR/libmotr-ut/
