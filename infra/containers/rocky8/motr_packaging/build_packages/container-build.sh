#!/bin/bash

#export MOTR_SRC_DIR=$(pwd)
#export MOTR_INSTALL_DIR=$MOTR_SRC_DIR/motr_install
#export MOTR_PACKAGE_IMAGE=motr_rocky8_build_packages 

docker run -it \
                -v $MOTR_SRC_DIR/cortx-motr/:/cortx-motr \
                -v $MOTR_SRC_DIR/cortx-hare/:/cortx-hare \
                -v $MOTR_SRC_DIR/cortx-utils/:/cortx-utils \
                -v $MOTR_INSTALL_DIR:/motr_install \
                $MOTR_PACKAGE_IMAGE
