#!/usr/bin/env bash

# set cmake path and test version
export PATH=/deps/cmake/bin:$PATH
cmake --version

# install deps
apt update
#             | For runing GVS         | For GVS tests   | For coverage reports
apt install -y xorg-dev libgl1-mesa-dev libgles2-mesa-dev lcov
