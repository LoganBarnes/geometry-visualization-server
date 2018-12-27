#!/usr/bin/env bash

function build_and_run {
    cmake -E make_directory $1
    cmake -E chdir $1 cmake -DCMAKE_BUILD_TYPE=$2 -DGVS_USE_DEV_FLAGS=ON -DGVS_BUILD_TESTS=ON ..
    cmake -E chdir $1 cmake --build . --parallel
    ./$1/bin/tests/all_gvs_tests
}

build_and_run cmake-build-debug Debug
build_and_run cmake-build-release Release
