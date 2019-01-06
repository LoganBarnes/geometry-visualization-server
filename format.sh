#!/usr/bin/env sh
FILE_LIST="$(find src -type f -name '*.cpp' -o -name '*.hpp')"
clang-format -i -style=file $FILE_LIST
