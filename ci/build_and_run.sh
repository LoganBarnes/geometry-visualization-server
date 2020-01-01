#!/usr/bin/env bash

set -e # fail script if any individual commands fail

function build_and_run() {
  cmake -E make_directory $1
  # Compile without tests enabled first
  cmake -E chdir $1 cmake .. -DCMAKE_BUILD_TYPE=$2 -DGVS_BUILD_EXAMPLES=ON -DGVS_USE_DEV_FLAGS=ON -DGVS_BUILD_TESTS=OFF
  cmake -E chdir $1 cmake --build . --parallel
  # Enable tests and rebuild
  cmake -E chdir $1 cmake .. -DCMAKE_BUILD_TYPE=$2 -DGVS_BUILD_EXAMPLES=ON -DGVS_USE_DEV_FLAGS=ON -DGVS_BUILD_TESTS=ON
  cmake -E chdir $1 cmake --build . --parallel
  # Run the tests
  cmake -E chdir $1 ctest
}

build_and_run docker-cmake-build-debug Debug
build_and_run docker-cmake-build-release Release

# Generate coverage reports
COVERAGE_TARGETS="$(find docker-cmake-build-debug/bin -name 'test_*' -exec basename {} \; | sed 's/[^ ]* */&_coverage/g')"
cmake -E chdir docker-cmake-build-debug cmake --build . --target ${COVERAGE_TARGETS}
