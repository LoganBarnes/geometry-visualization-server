#!/usr/bin/env bash

set -e # fail script if any individual commands fail

function build_and_run {
    cmake -E make_directory $1
    cmake -E chdir $1 cmake -DCMAKE_BUILD_TYPE=$2 -DGVS_BUILD_TESTS=ON -DGVS_USE_DEV_FLAGS=ON ..
    cmake -E chdir $1 cmake --build . --parallel
    cmake -E chdir $1 ctest
}

build_and_run cmake-build-debug Debug
# TODO run coverage command here
build_and_run cmake-build-release Release
