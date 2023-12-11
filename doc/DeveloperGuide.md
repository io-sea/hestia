# Developer Guide

This document covers more detailed Local Build and Test options for Hestia, as well as information on the CI.

- [Developer Guide](#developer-guide)
- [Local Build](#local-build)
  - [Testing](#testing)
    - [Unit and Integration Tests](#unit-and-integration-tests)
    - [E2E Tests](#e2e-tests)
    - [Linting and Static Analysis](#linting-and-static-analysis)
      - [Address Sanitizer](#address-sanitizer)
      - [Clang Format and Tidy](#clang-format-and-tidy)
        - [Mac Specifics](#mac-specifics)
    - [Code Coverage](#code-coverage)
  - [Packaging](#packaging)
    - [Packaging Debug Symbols](#packaging-debug-symbols)
    - [Packaging on Gitlab](#packaging-on-gitlab)
  - [Object Store Plugins](#object-store-plugins)
    - [Cortx-Motr](#cortx-motr)
    - [Phobos](#phobos)
- [CI](#ci)
  - [Merge Requests](#merge-requests)
  - [Nightly Builds](#nightly-builds)
  - [Version Bump and Release](#version-bump-and-release)
- [System Integration Testing](#system-integration-testing)
  - [S3](#s3)
    - [Minio](#minio)

# Local Build

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
* libxml2
* yaml-cpp
* spdlog
* CLI11
* Catch2

Optional dependencies for Plugins are:

* CEA Phobos
* Cortx Motr

## Testing

Tests are enabled with the `HESTIA_BUILD_TESTS` CMake option.

### Unit and Integration Tests
Unit and integration tests can be run with `make test`. 

### E2E Tests
E2E tests are driven with Python. They can be run with `make hestia_e2e_tests` and require the content of `test/e2e_tests/requirements.txt` to be installed as Python packages.

### Linting and Static Analysis

#### Address Sanitizer

The CMake option `HESTIA_ENABLE_SANITIZERS` will control whether gcc or clang address sanitizers (and similar tooling) is enabled. This is for Debug builds only.

#### Clang Format and Tidy

We use `clang-format` and `clang-tidy` for linting. To enable linting we need to build with a compile-commands database, the CMake option `cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON` provides this.

To do formatting you will need the `clang` compilers in your `PATH` and can then run:

```bash
infra/scripts/run_format.sh
```

and linting:

```bash
infra/scripts/run_lint.sh /path/to/build/dir
```

##### Mac Specifics

On Mac with `brew` we need to carefully set the runtime paths to avoid various compatibility issues:

```sh
brew install llvm
```

The following should be set in any shell you use for compiling (e.g. you can add them to `~/.zshrc`). Note this is slightly different to the paths suggested by brew.

```sh
export LLVM_PATH=/opt/homebrew/opt/llvm
export PATH=$LLVM_PATH/bin:$PATH
export LDFLAGS="-L$LLVM_PATH/lib/c++ -Wl,-rpath,$LLVM_PATH/lib/c++":$LDFLAGS
export CPPFLAGS=-I$LLVM_PATH/include:$CPPFLAGS
export CC=$LLVM_PATH/bin/clang
export CXX=$LLVM_PATH/bin/clang++
```

### Code Coverage

To enable code coverage targets, the CMake option `CODE_COVERAGE` needs to be set to `ON`, described further [here](https://git.ichec.ie/io-sea-internal/hestia/-/blob/devel/test/README.md).

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

### Packaging Debug Symbols

To package Debug symbols Hestia needs to be built with the `RelWithDebInfo` option and also in a build directory with a long path (longer than the eventual installation path). The reason for the latter is a limitation in CMake's path substitution approach when splitting symbols from the built binaries.

### Packaging on Gitlab

On the [Pipeline Schedules](https://git.ichec.ie/io-sea-internal/hestia/-/pipeline_schedules), there are three pipelines available that will create a release from the most recent tagged version on master.

These pipelines will automatically create an RPM, source RPM and TGZ of Hestia for Rocky8 and upload them to the Gitlab package registry. It will link these in the release page created.

## Object Store Plugins

Hestia provides interfaces to some object stores via Plugin - to support building the project on platforms not supported by those interfaces and for clean licensing.

### Cortx-Motr

To build the Cortx-Motr plugin, you will need `motr` installed. For convenience we provide RPMs for RHEL 8 in our [Gitlab package repository](https://git.ichec.ie/io-sea-internal/hestia/-/packages/33), with further build instructions [here](./plugins/Plugins.md).

### Phobos

Phobos is automatically found by CMake when `-DHESTIA_WITH_PHOBOS=ON` on Linux. 

If an installation is not found CMake will attempt to build it automatically. To do so it needs the following dependencies. 

```bash
dnf install -y autoconf automake libtool gcc-c++ git wget doxygen rpm-build python3-devel \
    make glib2-devel which jansson-devel libini_config-devel libattr-devel sg3_utils-devel protobuf-c-devel libpq-devel
```

Alternatively to CMake doing the build you can run the [following script](/infra/scripts/build_phobos.sh):

```bash
infra/scripts/build_phobos.sh infra/cmake/patches
``` 

to apply our patches and build Phobos. 

# CI

We use Gitlab for our CI https://git.ichec.ie/io-sea-internal/hestia

Our Gitlab configuration `.gitlab-ci.yml` is at the top level of the repo.

## Merge Requests
Contributions are via Gitlab Merge Request (MR) targeting the `devel` branch. Creating a MR will trigger the following build and test stages:

* `Format` - checks code formatting with `clang-format`
* `Lint` - static analysis with `clang-tidy`
* `Release` - do a Hestia build in Release configuration, including building an RPM
* `Unit Test` - build Hestia with static analyser and run unit and integration tests
* `E2E Test` - install Hestia as 1) a relocatable package (tar.gz) and 2) as a system package in a clean environment and run tests on our APIs

If the CI passes and you are happy with the change you can merge to devel. Reviews are encourage for larger changes or if you are unsure on anything.

## Nightly Builds
There is a collection of `Nightly` tests that run on the `devel` branch HEAD, if there have been any changes since the last run.

The have the following stages:

* `Build` - do the build, with some expensive dependencies (Phobos, Motr)
* `Unit Test` - run unit tests with the extra dependencies
* `E2E Test` - run e2e tests with the extra dependencies
* `Code Coverage` - run a code coverage job and store the results
* `Deploy` - Archive build outputs, bump the patch version and merge to `master`
* `Cleanup` - Clean up the build outputs

## Version Bump and Release

There are a collection of Deploy jobs that create a release from the `master` branch, with naming `Master_deploy_xxx`. They allow releasing while optionally incrementing major or minor version numbers.

They can be manually triggered through the Gitlab 'Build->Pipeline schedules' panel.

# System Integration Testing

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

