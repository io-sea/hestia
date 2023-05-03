FROM rockylinux:8

RUN yum check-update; yum update -y; yum install -y epel-release; yum config-manager --set-enabled powertools; \
    yum install -y autoconf automake libtool openssl-devel gcc-c++ git wget doxygen rpm-build python3-devel libxml2-devel libcurl-devel \
    make glib2-devel which jansson-devel libini_config-devel libattr-devel sg3_utils-devel protobuf-c-devel libpq-devel;

# CFLAGS=-Wno-error ./configure --disable-dependency-tracking --prefix=/io-sea/phobos-install