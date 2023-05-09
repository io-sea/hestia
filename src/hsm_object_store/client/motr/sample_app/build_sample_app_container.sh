mkdir sample_app_build
pushd sample_app_build
cmake -DMOTR_SRC_DIR=/cortx-motr /sample_app
make
popd

