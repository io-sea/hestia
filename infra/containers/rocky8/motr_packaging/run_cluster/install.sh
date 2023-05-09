export HOST_ARCH=$(arch)
export HOST_RELEASE=el8
export MOTR_VERSION=2.0.0-1_git71735b89
export HARE_VERSION=2.0.0-1_git9ade98b
export CORTX_UTILS_VERSION=2.0.0-2_95e10fa

yum install -y epel-release; yum config-manager --set-enabled powertools; yum install -y wget python3 procps-ng

pip3 install --upgrade pip
wget https://raw.githubusercontent.com/Seagate/cortx-utils/main/py-utils/python_requirements.txt
pip3 install -r python_requirements.txt
pip install cffi==1.14.5

yum config-manager --add-repo https://rpm.releases.hashicorp.com/RHEL/hashicorp.repo

cd motr_install
yum install -y isa-l-2.30.0-1.$HOST_RELEASE.$HOST_ARCH.rpm \
    cortx-py-utils-$CORTX_UTILS_VERSION.noarch.rpm \
    cortx-motr-$MOTR_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm \
    cortx-motr-devel-$MOTR_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm \
    cortx-hare-$HARE_VERSION.$HOST_RELEASE.$HOST_ARCH.rpm
   
dd if=/dev/zero of=/var/motr/disk0.img bs=1M seek=9999 count=1 
dd if=/dev/zero of=/var/motr/disk1.img bs=1M seek=9999 count=1

export PATH=$PATH:/opt/seagate/cortx/hare/bin/
groupadd --force hare