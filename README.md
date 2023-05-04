# Hestia

Hestia (Hierarchical Storage Tiers Interface for Applications) is a Hierarchical Storage Management (HSM) enabled interface to various Object Store backends.

It allows incorporation of both HSM-enabled and standard Object Store backend realisations into a single HSM API, with standard backends available as individual 'tiers'.

# Building from Source

Hestia is supported on Linux and Mac. 

## Dependencies

### Required

* A `c++17` compatible compiler
* `cmake >= 3.24` (script provided to fetch if system version too old)
* `curl` (and development headers)
* `libxml2` (and development headers)

### Recommended

* `OpenSSL` 

### Automatically Fetched If not Present

* `libs3` - for S3 clients
* `phobos` - for tape storage
* `Catch` - unit testing
* `spdlog` - logging
* `nlohmann:json` - json support

## Doing the build

It is a 'standard' CMake build, do:

```bash
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake $PATH_TO_SOURCE
make
```

# Applications
The main function can be run with `./hestia` from `/path/to/hestia/project/build/src`. Note that the executable must be run from this directory. 
