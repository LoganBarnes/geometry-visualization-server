#!/usr/bin/env bash

# set cmake path and test version
export PATH=/deps/cmake/bin:$PATH
cmake --version

# install deps
add-apt-repository ppa:ubuntu-toolchain-r/test -y
apt update
apt install -y gcc-7 g++-7 xorg-dev libgl1-mesa-dev uuid-dev lcov
gcc-7 --version
