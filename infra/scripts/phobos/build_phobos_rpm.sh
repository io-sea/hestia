export PHOBOS_VERSION="1.95.1"
wget https://github.com/cea-hpc/phobos/archive/refs/tags/$PHOBOS_VERSION.tar.gz
tar -xvf $PHOBOS_VERSION.tar.gz
cd phobos-$PHOBOS_VERSION/
./autogen.sh
cd ..
tar cvf phobos-$PHOBOS_VERSION.tar.gz phobos-$PHOBOS_VERSION/
cp phobos-$PHOBOS_VERSION.tar.gz /root/rpmbuild/SOURCES/
cd phobos-$PHOBOS_VERSION
./configure
rpmbuild -bb phobos.spec

# Wrote: /root/rpmbuild/RPMS/x86_64/phobos-1.95.1-1.el8.x86_64.rpm
# Wrote: /root/rpmbuild/RPMS/x86_64/phobos-devel-1.95.1-1.el8.x86_64.rpm
# Wrote: /root/rpmbuild/RPMS/x86_64/phobos-debugsource-1.95.1-1.el8.x86_64.rpm
# Wrote: /root/rpmbuild/RPMS/x86_64/phobos-debuginfo-1.95.1-1.el8.x86_64.rpm