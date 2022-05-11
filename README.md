# Hestia

Hierarchical Storage Tiers Interface for Applications

## Build Instructions

To configure and build Hestia, a C++17 compatible compiler is required, and CMake >= 3.0.

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

#### Setting installation directories
Option | Effect
------ | ------
`-DHESTIA_INSTALL_PREFIX=...` | Set to the root install directory for the compiled libraries and programs.
`-DHESTIA_INSTALL_BINDIR=...` | Set the install directory for the `hestia` executable. Use a relative path to set the path relative to `${CMAKE_INSTALL_PREFIX}`. (Default `bin`).
`-DHESTIA_INSTALL_LIBDIR=...` | Set the install directory for `hestia` libraries. Use a relative path to set the path relative to `${CMAKE_INSTALL_PREFIX}`. (Default `lib`).

## Testing

Units test can be run with `make -C build tests`, given `HESTIA_BUILD_TESTS` was set to `ON` during the build process.
