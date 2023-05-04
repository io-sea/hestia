# Configurations

## Coverage

To enable coverage set the CMake option `CODE_COVERAGE` to `ON`.

### Mac/Clang

`llvm` and `lcov` pacakages are needed.

On Mac do:

```bash
brew install llvm lcov
```

`llvm` is installed to `/opt/homebrew/opt/llvm/`, the `bin` and `lib` directories need to be in the `PATH`.

### Running

Run `ccov-exeName` to run the coverage wrapper for `exeName`.