#!/bin/sh

script="${BASH_SOURCE[0]:-${(%):-%x}}"
script_dir="$( cd "$( dirname "${script}" )" >/dev/null 2>&1 && pwd )"
build_dir="${script_dir}/build"
doc_dir="${script_dir}/doc/parser/src"

mkdir -p ${build_dir}
pushd ${build_dir}

cmake \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DHESTIA_BUILD_TESTS=ON \
    -DHESTIA_BUILD_DOCUMENTATION=ON \
    -DCUSTOM_DOCU_PATH="${doc_dir}" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=g++ \
    "${script_dir}"

make -j 4

popd # build_dir
