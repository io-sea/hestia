FROM rockylinux:8

RUN yum check-update; yum update -y; yum install -y epel-release; yum config-manager --set-enabled powertools; \
    yum install -y ansible autoconf automake libtool castxml  \
    libyaml libyaml-devel systemd-devel libuuid libuuid-devel libedit-devel \
    libaio-devel openssl-devel gcc-c++ yum-utils libfabric libxslt libfabric-devel rpm-build \
    asciidoc docbook-dtds docbook-style-xsl python3-ply python3-devel python3-numpy freetype-devel \
    binutils-devel facter git libffi-devel nasm yasm\
    perl perl-autodie perl-File-Slurp perl-IO-All perl-File-Find-Rule perl-YAML-LibYAML perl-List-MoreUtils perl-XML-LibXML;\
    yum clean all; rm -rf /var/cache/yum;

RUN python3 -m pip install --upgrade pip; python3 -m pip install --no-cache-dir asciidoc ply

COPY build.sh .

CMD ./build.sh

 
