mkdir motr_superbuild
cd motr_superbuild
git clone --recursive https://github.com/Seagate/cortx-motr.git
git clone https://github.com/Seagate/cortx-hare.git
git clone https://github.com/Seagate/cortx-utils.git
export MOTR_SRC_DIR=$(pwd)
mkdir motr_install
export MOTR_INSTALL_DIR=$MOTR_SRC_DIR/motr_install
cd ..
