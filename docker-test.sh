#!/usr/bin/env bash

# All compiled dependencies are installed in /deps/
DEPS_DIR=/deps
mkdir -p ${DEPS_DIR} && cd ${DEPS_DIR}

CMAKE_URL="https://cmake.org/files/v3.15/cmake-3.15.0-Linux-x86_64.tar.gz"
mkdir cmake && wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
export PATH=${DEPS_DIR}/cmake/bin:${PATH}
cmake --version

cd /gvs
. ./ci/install_deps.sh
./ci/build_and_run.sh
