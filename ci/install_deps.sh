#!/usr/bin/env bash

# set cmake path and test version
export PATH=/deps/cmake/bin:$PATH
cmake --version

# install deps
apt update
apt install -y xorg-dev libgl1-mesa-dev uuid-dev #libxrandr-dev libxinerama-dev libxcursor-dev
