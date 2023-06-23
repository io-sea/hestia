#!/usr/bin/env bash
export MOTR_SHA=71735b89b26fec78cea0f5fb43dadd964486fff5;

cd $CI_PROJECT_DIR;
curl $HESTIA_API_URL/packages/generic/motr-rpms/1.0.0/motr-rpms-$(arch).tar.gz | tar xz;
motr_rpms=$CI_PROJECT_DIR/builds/io-sea-internal/hestia/motr-rpms;
yum install -qy $motr_rpms/isa-l-2.30.0*.$(arch).rpm;
yum install -qy $motr_rpms/cortx-motr-2.0.0*.$(arch).rpm;
yum install -qy $motr_rpms/cortx-motr-devel-2.0.0*.$(arch).rpm;
git clone --recursive https://github.com/Seagate/cortx-motr.git $MOTR_SRC_DIR;
pushd $MOTR_SRC_DIR && git reset --hard $MOTR_SHA && popd;