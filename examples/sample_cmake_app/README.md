# Hestia Sample App

This is a sample app demonstrating the Hestia C API use with CMake. To build do:

```bash
cmake $PATH_TO_SOURCE
make
```

If Hestia is installed to a non-standard location do:

```bash
cmake -Dhestia_DIR=$HESTIA_INSTALL_DIR/lib/cmake/hestia $PATH_TO_SOURCE
make
```

This will build C and C++ sample apps, which can be run with `./hestia_sample_app_c` and `./hestia_sample_app_cpp`.

The C++ sample uses a modern C++(17) syntax, if your compiler doesn't support it you can build just the C app by adding the `-DWITH_CPP_APP=OFF` option to the cmake invocation.