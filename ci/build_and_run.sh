#!/usr/bin/env bash

set -e # fail script if any individual commands fail

#function build_and_run {
#    cmake -E make_directory $1
#    cmake -E chdir $1 cmake -DCMAKE_BUILD_TYPE=$2 -DGVS_USE_DEV_FLAGS=ON -DGVS_BUILD_TESTS=ON ..
#    cmake -E chdir $1 cmake --build . --parallel
#    ./$1/bin/tests/all_gvs_tests
#}
#
#build_and_run cmake-build-debug Debug
#build_and_run cmake-build-release Release

cmake -E make_directory cmake-build-debug
cmake -E chdir cmake-build-debug cmake -DCMAKE_BUILD_TYPE=Debug -DGVS_USE_DEV_FLAGS=ON -DGVS_BUILD_TESTS=ON ..
cmake -E chdir cmake-build-debug cmake --build . --target gvs_coverage --parallel
