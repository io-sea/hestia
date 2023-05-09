export HOST_ARCH=$(arch)
export HOST_RELEASE=el8
export RPM_CACHE=/root/rpmbuild/RPMS/$HOST_ARCH/
export MOTR_VERSION=2.0.0-1_git71735b89
export HARE_VERSION=2.0.0-1_git9ade98b
export CORTX_UTILS_VERSION=2.0.0-2_95e10fa

rpmbuild --define '_isa_l_version 2.30.0' --bb /cortx-motr/scripts/provisioning/roles/motr-build/tasks/isa-l.spec
yum install -y $RPM_CACHE/isa-l-2.30.0-1.$HOST_RELEASE.$HOST_ARCH.rpm

cd /cortx-motr
./autogen.sh
./configure --prefix=/motr_install --with-user-mode-only
make -j$(nproc)
export PYTHONPATH=$PYTHONPATH:/usr/local/lib/python3.6/site-packages/
make install -j$(nproc)
make rpms -j$(nproc)

cp -f $RPM_CACHE/* /motr_install
yum install -y $RPM_CACHE/cortx-motr-$MOTR_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm
yum install -y $RPM_CACHE/cortx-motr-devel-$MOTR_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm

if [ $HOST_ARCH == "aarch64" ];then
# On aarch64 nothing gives kafka python wheel and repo lib is too old, so need to build.
cd /;git clone https://github.com/confluentinc/librdkafka.git;cd librdkafka
./configure --install-deps;make;make install
python3 -m pip install confluent-kafka==1.5.0
fi 

cd /cortx-utils
./jenkins/build.sh -v 2.0.0 -b 2 -j$(nproc)
pip3 install -r py-utils/python_requirements.txt
pip3 install cffi==1.14.5 numpy==1.19.5
yum install -y /cortx-utils/py-utils/dist/cortx-py-utils-$CORTX_UTILS_VERSION.noarch.rpm
cp -f /cortx-utils/py-utils/dist/cortx-py-utils-$CORTX_UTILS_VERSION.noarch.rpm /motr_install

# On aarch64 we fail here at the last hurdle - the python wheel name is hard coded to x86_64 in hax
if [ $HOST_ARCH != "aarch64" ];then
cd /cortx-hare
make rpm -j$(nproc)
yum install -y $RPM_CACHE/cortx-hare-$HARE_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm
fi

cp -f $RPM_CACHE/* /motr_install
