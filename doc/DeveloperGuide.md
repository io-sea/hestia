# Developer Guide

This document covers more detailed Local Build and Test options for Hestia, as well as information on the CI.

- [Developer Guide](#developer-guide)
- [Local build guide](#local-build-guide)
  - [Testing](#testing)
  - [Linting and Static Analysis](#linting-and-static-analysis)
  - [Packaging](#packaging)
    - [Packaging on Gitlab](#packaging-on-gitlab)
  - [Object Store Plugins](#object-store-plugins)
    - [Cortx-Motr](#cortx-motr)
    - [Phobos](#phobos)
- [Integration Testing](#integration-testing)
  - [S3](#s3)
    - [Minio](#minio)

# Local build guide

Hestia is built with a recent version of CMake (>=3.24). If you system provided package is too old CMake can be downloaded as a standalone executable, we have a script [here](/infra/scripts/bootstrap_cmake.sh) to help.


```bash
# Make a directory to hold the build files and change to it
mkdir build; cd build

# Configure the Hestia build using CMake
cmake /path/to/hestia/source

# Build Hestia
make build 

# Install Hestia 
make install
```

During the build several third-party libraries are downloaded and built if system versions are not found. You can also build against the system versions in most cases. The main dependencies are:

* OpenSSL
* cURL
* nlohmann JSON
* hiredis
* yaml-cpp
* spdlog
* CLI11
* Catch2

Optional dependencies for Plugins are:

* libxml2
* libS3
* Proxygen
* CEA Phobos
* Cortx Motr


## Testing

Units test can be run with `make test`, assuming `HESTIA_BUILD_TESTS` was set to `ON` during the build process. To enable code coverage targets, the CMake option `CODE_COVERAGE` needs to be set to `ON`, described further [here](https://git.ichec.ie/io-sea-internal/hestia/-/blob/devel/test/README.md).

## Linting and Static Analysis

We use `clang-format` and `clang-tidy` for linting. To enable linting we need to build with a compile-commands database, the CMake option `cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON` provides this.

On Mac `brew` currently packages `clang-format` and `llvm` as version 16, which are newer than the CI version. This can lead to compatibility issues. For us, version 15 works:

```bash
brew install llvm@15
```

If you want to use this as your default compiler add the following to your `.zshrc`.

```bash
echo 'export PATH=$LLVM_PATH/bin:$PATH' >> ~/.zshrc
echo 'export LDFLAGS=-L$LLVM_PATH/lib' >> ~/.zshrc
echo 'export CPPFLAGS=-I$LLVM_PATH/include' >> ~/.zshrc
```

A typical value on Mac is `LLVM_PATH=/opt/homebrew/opt/llvm@15`;

Before running CMake, the compiler should be explicitly set to this one also:

```bash
export CC=$LLVM_PATH/bin/clang
export CXX=$LLVM_PATH/bin/clang++
```

To do formatting you can run:

```bash
infra/scripts/run_format.sh
```

and linting:

```bash
infra/scripts/run_lint.sh /path/to/build/dir
```

## Packaging

Hestia can be packaged with:

```bash
make package
```

which will build an RPM and/or TGZ depending on the platform. The CMake Option `BUILD_SHARED_LIBS` should be `OFF` when building the package - otherwise library link dependencies will be incorrect.

Hestia's source code can be packaged as a source RPM/TGZ by running: 

```bash
cmake -DHESTIA_USE_SRC_RPM_SPEC=ON .
make package_source
```

The extra CMake flag is to use a custom spec file, it should be turned off again if building a binary RPM.

### Packaging on Gitlab

On the [Pipeline Schedules](https://git.ichec.ie/io-sea-internal/hestia/-/pipeline_schedules), there are three pipelines available that will create a release from the most recent tagged version on master.

These pipelines will automatically create an RPM, source RPM and TGZ of Hestia for Rocky8 and upload them to the Gitlab package registry. It will link these in the release page created.

## Object Store Plugins

Hestia provides interfaces to some object stores via Plugin - to support building the project on platforms not supported by those interfaces and for clean licensing.

### Cortx-Motr

To build the Cortx-Motr plugin, you will need `motr` installed. For convenience we provide RPMs for RHEL 8 in our [Gitlab package repository](https://git.ichec.ie/io-sea-internal/hestia/-/packages/33), with further build instructions [here](/doc/Markdown/motr_use.md).

### Phobos

Phobos is automatically found by CMake when `-DHESTIA_WITH_PHOBOS=ON`. Note, this will silently fail on a Mac and not look for phobos. 

To build Phobos, you need to install the following dependencies. 

```bash
yum install -y autoconf automake libtool openssl-devel gcc-c++ git wget doxygen rpm-build python3-devel libxml2-devel libcurl-devel \
    make glib2-devel which jansson-devel libini_config-devel libattr-devel sg3_utils-devel protobuf-c-devel libpq-devel
```

Then you can run the [following script](/infra/scripts/build_phobos.sh) as follows:

```bash
infra/scripts/build_phobos.sh infra/cmake/patches
``` 

to apply our patches and build Phobos.

# Integration Testing

This section documents some setup to help with testing Hestia against third-party applications.

## S3

### Minio

Minio provides several object store interfaces. For us it is useful as an S3 server for running our S3 client against.

```bash
# https://min.io/download#/macos
brew install minio/stable/minio
MINIO_ROOT_USER=admin MINIO_ROOT_PASSWORD=password minio server /mnt/data --console-address ":9001"
```

From there you can log into the admin console, create access tokens, buckets and objects.

