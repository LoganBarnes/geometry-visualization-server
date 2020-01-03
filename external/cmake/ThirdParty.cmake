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
FetchContent_Declare(
        expected_dl
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG v1.0.0
)
FetchContent_Declare(
        mapbox_dl
        GIT_REPOSITORY https://github.com/mapbox/variant.git
        GIT_TAG v1.1.6
)
FetchContent_Declare(
        doctest_dl
        GIT_REPOSITORY https://github.com/onqtam/doctest.git
        GIT_TAG 2.3.6
)
FetchContent_Declare(
        range_v3_dl
        GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
        GIT_TAG 0.9.1
)

### Boost UUID ###
include(cmake/BoostUuidLibs.cmake)

### Expected ###
FetchContent_GetProperties(expected_dl)
if (NOT expected_dl_POPULATED)
    FetchContent_Populate(expected_dl)

    set(EXPECTED_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(EXPECTED_ENABLE_DOCS OFF CACHE BOOL "" FORCE)

    # compile expected with current project
    add_subdirectory(${expected_dl_SOURCE_DIR} ${expected_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif (NOT expected_dl_POPULATED)

### mapbox variant ###
FetchContent_GetProperties(mapbox_dl)
if (NOT mapbox_dl_POPULATED)
    FetchContent_Populate(mapbox_dl)

    add_library(mapbox INTERFACE)
    target_include_directories(mapbox SYSTEM INTERFACE "$<BUILD_INTERFACE:${mapbox_dl_SOURCE_DIR}/include>")
endif (NOT mapbox_dl_POPULATED)

### Range-v3 ###
FetchContent_GetProperties(range_v3_dl)
if (NOT range_v3_dl_POPULATED)
    FetchContent_Populate(range_v3_dl)

    set(RANGES_CXX_STD 17 CACHE INTERNAL "C++ standard version")

    # compile with current project
    add_subdirectory(${range_v3_dl_SOURCE_DIR} ${range_v3_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
    target_include_directories(range-v3 SYSTEM INTERFACE $<BUILD_INTERFACE:${range_v3_dl_SOURCE_DIR}/include/>)
endif (NOT range_v3_dl_POPULATED)

### DocTest ###
FetchContent_GetProperties(doctest_dl)
if (NOT doctest_dl_POPULATED)
    FetchContent_Populate(doctest_dl)

    set(DOCTEST_WITH_TESTS OFF CACHE BOOL "" FORCE)
    set(DOCTEST_WITH_MAIN_IN_STATIC_LIB ON CACHE BOOL "" FORCE)

    # compile with current project
    add_subdirectory(${doctest_dl_SOURCE_DIR} ${doctest_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    # Threads #
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(doctest INTERFACE Threads::Threads)

    # add test coverage capabilities if available
    find_program(LCOV_EXE
            NAMES "lcov"
            DOC "Path to lcov executable"
            )

    if (LCOV_EXE AND CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE MATCHES "Debug")
        include(cmake/CodeCoverage.cmake)
    endif ()
endif ()
