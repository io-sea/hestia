script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export MOTR_PACKAGE_DIR=$script_dir/build_packages

pushd ${MOTR_PACKAGE_DIR}

export MOTR_PACKAGE_IMAGE=motr_rocky8_build_packages
docker build -t $MOTR_PACKAGE_IMAGE .

popd 

mkdir cortx_packages 
export $MY_WORK_DIR=$script_dir/cortx_packages

cd $MY_WORK_DIR

source $MOTR_PACKAGE_DIR/setup_build_directory.sh
source $MOTR_PACKAGE_DIR/container-build.sh
