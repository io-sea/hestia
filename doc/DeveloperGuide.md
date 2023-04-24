# Running Static Analysis

## Clang Format

### Dependencies

#### Mac

On Mac `brew` currently packages `clang-format` as version 16, which is newer than the CI version and has compatibility issues. If you have already installed that you can unlink it with `brew unlink clang-format`. 

For us, version 11 works. You can do `clang-format@11` with `brew`:

```bash
brew install clang-format@11
```

### Run

The format script is `tools/run_format.sh`, which in turn runs `tools/format.sh`. In that file if you are on Mac you need to explicitly enter the path to `clang-format@11` as the clang exe (something like: `/opt/homebrew/opt/clang-format@11/bin/clang-format-11`). After that you can run:

```bash
tools/run_format.sh
```

which will update files with any needed changes.

## Clang Tidy

### Dependencies

#### Mac

It comes with `llvm`:

```bash
brew install llvm
```

### Prepare build

The build needs to be configured with the brew llvm version of `clang` rather than the `clang` packaged with Mac. You can point cmake to the chosen one by doing (e.g. on Mac):

```bash
export CC=/opt/homebrew/opt/llvm/bin/clang
export CXX=/opt/homebrew/opt/llvm/bin/clang++
```

Now you can run cmake in with a **clean** build directory (`$BUILD_DIR`) in the same shell session. It needs to exports its compile commands, so do:

```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON $SOURE_DIR
```

### Run

You can do the following the source directory, pointing to the `$BUILD_DIR`:

```bash
tools/run_lint.sh $BUILD_DIR
```





