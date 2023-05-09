#!/bin/bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export MOTR_DEV_ENV_DIR=$script_dir/run_dev_environment

pushd $MOTR_DEV_ENV_DIR
export MOTR_DEV_ENV_IMAGE=motr_rocky8_dev_env
docker build -t $MOTR_DEV_ENV_IMAGE .
popd

export MOTR_SRC_DIR=$script_dir/cortx_packages/motr_superbuild
export MOTR_INSTALL_DIR=$MOTR_SRC_DIR/motr_install

export SAMPLE_APP_DIR=$script_dir/../../../../src/hsm_object_store/client/motr/sample_app

docker run -it -v $MOTR_INSTALL_DIR:/motr_install -v $MOTR_SRC_DIR/cortx-motr:/cortx-motr -v $script_dir:/motr_packaging -v $SAMPLE_APP_DIR:/sample_app $MOTR_DEV_ENV_IMAGE /bin/bash
