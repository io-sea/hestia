#!/usr/bin/env bash
# Set local install location
if [[ $# -ge 1 ]]
then
    CMAKE_PREFIX=$1
else
    CMAKE_PREFIX="$(pwd)/cmake"
fi

# Checks if we have a local installation already
if [[ -f "${CMAKE_PREFIX}/bin/cmake" ]]; then
    LOCAL_CMAKE_VERSION=$($CMAKE_PREFIX/bin/cmake --version | grep -Eo "3\.[2-9][4-9](\.[0-9]+)*")
    if [[ -n $LOCAL_CMAKE_VERSION  ]]; then
        echo "Local CMake v$LOCAL_CMAKE_VERSION is installed at $CMAKE_PREFIX, exiting."
        exit
    fi
fi 

# Checks if system version is sufficient
if type -p cmake > /dev/null; then 
    SYS_CMAKE_VERSION=$( cmake --version | grep -Eo "3\.[2-9][4-9](\.[0-9]+)*" )
    echo "System CMake is v$(cmake --version | grep -Eo '[0-9]+(\.[0-9]+)*')"
    
    if [[ -n $SYS_CMAKE_VERSION ]]; then
        echo "CMake $SYS_CMAKE_VERSION is installed at $(type -p cmake), exiting."
        exit
    fi
fi 

echo "CMake version >= 3.24.* required, installing..."

ARCH=$(arch)
CMAKE_VERSION="3.26.3" # Most recent version
CMAKE_ARCHIVE_NAME="cmake-$CMAKE_VERSION-linux-$ARCH"

echo "Installing $CMAKE_ARCHIVE_NAME to $CMAKE_PREFIX"
mkdir -p $CMAKE_PREFIX
wget "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/$CMAKE_ARCHIVE_NAME.sh" -nv --no-check-certificate
sh $CMAKE_ARCHIVE_NAME.sh --prefix=$CMAKE_PREFIX --skip-license
rm -rf $CMAKE_ARCHIVE_NAME.sh
