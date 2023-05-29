# syntax=docker/dockerfile:1
# General setup
ARG OS_VERSION=8
FROM rockylinux:${OS_VERSION}
LABEL description="Hestia rockylinux:8 CI build environment"

# Install dependencies 
RUN yum check-update; yum update -y; \
    yum install -y epel-release; \
    yum config-manager --set-enabled powertools;

ARG HESTIA_ROOT=/hestia
COPY infra/deps/rocky8 ${HESTIA_ROOT}/deps/
RUN yum install -y $(cat ${HESTIA_ROOT}/deps/base);
COPY infra/scripts/bootstrap_cmake.sh  ${HESTIA_ROOT}/
RUN  ${HESTIA_ROOT}/bootstrap_cmake.sh /usr/local;

RUN yum install -y $(cat ${HESTIA_ROOT}/deps/format);
RUN yum install -y $(cat ${HESTIA_ROOT}/deps/lint); 
RUN yum install -y $(cat ${HESTIA_ROOT}/deps/test); 
RUN yum install -y $(cat ${HESTIA_ROOT}/deps/e2e_test);

COPY test/e2e_tests/requirements.txt ${HESTIA_ROOT}/test/e2e_tests/requirements.txt
RUN pip3 install -r ${HESTIA_ROOT}/test/e2e_tests/requirements.txt;

RUN yum install -y $(cat ${HESTIA_ROOT}/deps/phobos); 

RUN mkdir ${HESTIA_ROOT}/motr && curl https://git.ichec.ie/api/v4/projects/418/packages/generic/motr-rpms/1.0.0/motr-rpms-$(arch).tar.gz | tar xz -C ${HESTIA_ROOT}/motr; \
    export MOTR_RPM_DIR=${HESTIA_ROOT}/motr/builds/io-sea-internal/hestia/motr-rpms; \
    yum install -y  $MOTR_RPM_DIR/isa-l-2.30.0*.$(arch).rpm \
                    $MOTR_RPM_DIR/cortx-motr-2.0.0*.$(arch).rpm \
                    $MOTR_RPM_DIR/cortx-motr-devel-2.0.0*.$(arch).rpm;  

RUN export MOTR_VERSION=2.0.0-1_git71735b89; \
    export MOTR_SHA=71735b89b26fec78cea0f5fb43dadd964486fff5; \
    export MOTR_SRC=/motr/motr-src; \
    git clone --recursive https://github.com/Seagate/cortx-motr.git $MOTR_SRC; \
    pushd $MOTR_SRC && git reset --hard $MOTR_SHA && popd;

# Reduce image size
RUN yum clean all; rm -rf /var/cache/yum; \
    rm -rf ${HESTIA_ROOT}
