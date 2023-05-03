SOURCE_DIR=$1
CONTAINER_NAME=$2

CONTAINER_DIR="${CONTAINER_NAME}_container_build"

mkdir -p $CONTAINER_DIR
cd $CONTAINER_DIR
cp -r $SOURCE_DIR/infra/cmake/patches/ .
cp $SOURCE_DIR/infra/scripts/build_boost.sh .
cp $SOURCE_DIR/infra/scripts/build_phobos.sh .
cp -r $SOURCE_DIR/infra/containers/$CONTAINER_NAME/Dockerfile .

docker build -t deimos_$CONTAINER_NAME .