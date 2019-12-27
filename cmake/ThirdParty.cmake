##########################################################################################
# Geometry Visualization Server
# Copyright (c) 2019 Logan Barnes - All Rights Reserved
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
##########################################################################################
include(FetchContent)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

### gRPC Wrapper Library ###
FetchContent_Declare(
        grpcw_dl
        GIT_REPOSITORY https://github.com/LoganBarnes/grpc-wrapper.git
        GIT_TAG v1.0.0
)

FetchContent_GetProperties(grpcw_dl)
if (NOT grpcw_dl_POPULATED)
    FetchContent_Populate(grpcw_dl)

    add_subdirectory(${grpcw_dl_SOURCE_DIR} ${grpcw_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

### Expected ###
FetchContent_Declare(
        expected_dl
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG v1.0.0
)

FetchContent_GetProperties(expected_dl)
if (NOT expected_dl_POPULATED)
    FetchContent_Populate(expected_dl)

    set(EXPECTED_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(EXPECTED_ENABLE_DOCS OFF CACHE BOOL "" FORCE)

    # compile expected with current project
    add_subdirectory(${expected_dl_SOURCE_DIR} ${expected_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif (NOT expected_dl_POPULATED)

### stduuid ###
FetchContent_Declare(
        boost_uuid_dl
        GIT_REPOSITORY https://github.com/boostorg/uuid.git
        GIT_TAG boost-1.72.0
)

FetchContent_GetProperties(boost_uuid_dl)
if (NOT boost_uuid_dl_POPULATED)
    FetchContent_Populate(boost_uuid_dl)

    add_library(boostuuid INTERFACE)
    target_include_directories(boostuuid SYSTEM INTERFACE "$<BUILD_INTERFACE:${boost_uuid_dl_SOURCE_DIR}/include>")
endif (NOT boost_uuid_dl_POPULATED)
