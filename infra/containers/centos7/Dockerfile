FROM centos:7

RUN yum check-update; yum update -y; yum install -y epel-release centos-release-scl;\
    yum install -y autoconf automake libtool gcc-c++ git make wget which doxygen rpm-build\
    devtoolset-11-gcc devtoolset-11-gcc-c++ libcurl-devel openssl-devel libxml2-devel\
    python3-devel glib2-devel jansson-devel libini_config-devel libattr-devel sg3_utils-devel protobuf-c-devel rh-postgresql10-postgresql-devel;

COPY build_phobos.sh /
ENV PKG_CONFIG_PATH=/opt/rh/rh-postgresql10/root/usr/lib64/pkgconfig
RUN mkdir build; cd build; ../build_phobos.sh; cd ..;scl enable devtoolset-11 bash