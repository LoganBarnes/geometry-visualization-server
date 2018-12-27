#!/usr/bin/env bash
# set cmake path and test version
export PATH=/deps/cmake/bin:$PATH
cmake --version

# install deps
apt update
apt install gcc-7
