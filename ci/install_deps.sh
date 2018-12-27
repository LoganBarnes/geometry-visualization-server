#!/usr/bin/env bash

############################################################################
# All compiled dependencies are installed in ${TRAVIS_BUILD_DIR}/deps/
############################################################################
DEPS_DIR="deps"
mkdir -p ${DEPS_DIR} && cd ${DEPS_DIR}

############################################################################
# Install a recent CMake
############################################################################
CMAKE_URL="https://cmake.org/files/v3.12/cmake-3.12.0-Linux-x86_64.tar.gz"
mkdir cmake && travis_retry wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
export PATH=${DEPS_DIR}/cmake/bin:${PATH}

cmake --version
