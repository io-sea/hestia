# Proxygen

Proxygen is a C++ webserver libary by Facebook https://github.com/facebook/proxygen

At the time of writing it is not packaged on `hestia` target platforms - so needs to be built from source.

The `hestia` project builds Proxygen (if an installation is not found) and several of its dependencies. System installs of some other dependencies are still needed though. They are as follows:

## Mac

```bash
brew install boost libsodium double-conversion googletest glog zstd
```