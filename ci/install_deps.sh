#!/usr/bin/env bash

# set cmake path and test version
export PATH=/deps/cmake/bin:$PATH
cmake --version

# install deps
apt update
apt install -y software-properties-common # installs add-apt-repository

# ppa for gcc-7
add-apt-repository ppa:ubuntu-toolchain-r/test -y
apt update
apt install -y gcc-7 g++-7 xorg-dev libgl1-mesa-dev uuid-dev lcov
gcc-7 --version
