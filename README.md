# Hestia

Hierarchical Storage Tiers Interface for Applications. This is version 0.1 completed in December 2022. It handles data movements and managing metadata for a single object. 

## Requirements

To configure and build Hestia, following are required. 
- C++17 compatible compiler
- CMake >= 3.0
- nlohmann-json-devel 

Optional dependencies:
- catch2 (for tests)
- doxygen (for documentation)

## Build Instructions


```
# Make a directory to hold the temporary build files
mkdir -p build

# Change directory to the build directory
cd build

# Configure the Hestia build using CMake
cmake \
    [options below] \
    /path/to/hestia/project

cd /path/to/hestia/project

# Build the Hestia project
make -C build 

# Install the Hestia executable
make -C build install
```

### Useful Options

#### Setting Compile/Link Flags
Option | Effect
------ | ------
`-DCMAKE_CXX_COMPILER=...` | Set the C++ compiler.
`-DCMAKE_CXX_FLAGS=...`    | Set the flags to pass to the C++ compiler. Overrides the default flags.
`-DCMAKE_BUILD_TYPE=...`   | Set to `Release` or `Debug` for release or debug builds. Sets a number of flags by default.

#### Enabling/Disabling Sections of Hestia
Option | Effect
------ | ------
`-DHESTIA_BUILD_TESTS=...`     | Set to `ON` to build Hestia tests and enable the `make test` target, or `OFF` to skip (Default `OFF`).
`-DHESTIA_BUILD_DOCUMENTATION=...`     | Set to `ON` to generate doxygen documentation (Default `OFF`).

#### Setting installation directories
Option | Effect
------ | ------
`-DCMAKE_INSTALL_PREFIX=...` | Set to the root install directory for the compiled libraries and programs.
`-DCMAKE_INSTALL_BINDIR=...` | Set the install directory for the `hestia` executable. Use a relative path to set the path relative to `${CMAKE_INSTALL_PREFIX}`. (Default `bin`).
`-DCMAKE_INSTALL_LIBDIR=...` | Set the install directory for `hestia` libraries. Use a relative path to set the path relative to `${CMAKE_INSTALL_PREFIX}`. (Default `lib`).

## Testing

Units test can be run with `make -C build test`, given `HESTIA_BUILD_TESTS` was set to `ON` during the build process. 

## Running
The main function can be run with `./hestia` from `/path/to/hestia/project/build/src`. Note that the executable must be run from this directory. 
