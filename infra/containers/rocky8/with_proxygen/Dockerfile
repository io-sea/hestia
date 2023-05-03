FROM rockylinux:8

RUN yum check-update; yum update -y; yum install -y epel-release; yum config-manager --set-enabled powertools; \
    yum install -y autoconf automake libtool openssl-devel gcc-c++ git wget rpm-build python3-devel libxml2-devel libcurl-devel \
    boost-devel gflags-devel libzstd-devel libsodium-devel libevent-devel double-conversion-devel gperf glog-devel\
    make glib2-devel which jansson-devel libini_config-devel libattr-devel sg3_utils-devel protobuf-c-devel libpq-devel;

COPY build_phobos.sh /
COPY patches /
RUN mkdir build; cd build; ../build_phobos.sh; cd ..; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/phobos;