# syntax=docker/dockerfile:1
FROM hestia/ci-base:latest
LABEL description="Hestia rockylinux:8 CI build environment with cortx-motr"

RUN mkdir /motr && curl https://git.ichec.ie/api/v4/projects/418/packages/generic/motr-rpms/1.0.0/motr-rpms-$(arch).tar.gz | tar xz -C /motr; \
    export MOTR_RPM_DIR=/motr/builds/io-sea-internal/hestia/motr-rpms; \
    yum install -y  $MOTR_RPM_DIR/isa-l-2.30.0*.$(arch).rpm \
                    $MOTR_RPM_DIR/cortx-motr-2.0.0*.$(arch).rpm \
                    $MOTR_RPM_DIR/cortx-motr-devel-2.0.0*.$(arch).rpm;  

RUN export MOTR_VERSION=2.0.0-1_git71735b89; \
    export MOTR_SHA=71735b89b26fec78cea0f5fb43dadd964486fff5; \
    export MOTR_SRC=/motr/motr-src; \
    git clone --recursive https://github.com/Seagate/cortx-motr.git $MOTR_SRC; \
    pushd $MOTR_SRC && git reset --hard $MOTR_SHA && popd;
