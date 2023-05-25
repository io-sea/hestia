Sample of building a motr client.

Needs motr to be installed. For a system installed motr do:

```bash
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake $SRC_DIR
make
```

If it is installed in a non-standard location do:

```bash
cmake -DCMAKE_PREFIX_PATH=/motr_install_dir $SRC_DIR
make
```