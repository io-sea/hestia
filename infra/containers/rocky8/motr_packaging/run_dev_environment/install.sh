export HOST_ARCH=$(arch)
export HOST_RELEASE=el8
export MOTR_VERSION=2.0.0-1_git71735b89

cd motr_install
yum install -y isa-l-2.30.0-1.$HOST_RELEASE.$HOST_ARCH.rpm \
    cortx-motr-$MOTR_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm \
    cortx-motr-devel-$MOTR_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm

# Missing from rpm and rpm-test target is broken
cp /motr_install/lib/libmotr-ut.so* /usr/lib64