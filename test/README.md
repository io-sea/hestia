# Configurations

## Coverage

Code coverage CMake module: [https://github.com/StableCoder/cmake-scripts/blob/main/code-coverage.cmake](https://github.com/StableCoder/cmake-scripts/blob/main/code-coverage.cmake). To enable coverage set the CMake option `CODE_COVERAGE` to `ON`.

### Mac/Clang

`llvm` and `lcov` pacakages are needed.

On Mac do:

```bash
brew install llvm lcov
```

The `bin` and `lib` directories need to be in the `PATH`.

### Building

Set the C and C++ compiler paths. 

```bash
export CC=/path/to/llvm/source/bin/clang
export CXX=/path/to/llvm/source/bin/clang++
```

Run cmake in with `-DHESTIA_BUILD_TESTS=ON -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug`.

Note: You can also compile with gcc and produce code coverage report with lcov. This is default in CI.

### Running

Run `make ccov-exeName` to run the coverage wrapper for `exeName` where `exeName` is an executable target, for example, `make ccov-hestia_unit_tests`. 

`ccov-all` generates HTML code coverage report, merging every target added with 'ALL' parameter.

### View the results

HTML report was genereted as `$BUILD_DIR/ccov/all-merged/index.html`.

