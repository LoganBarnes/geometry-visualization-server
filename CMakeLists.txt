##########################################################################################
# Geometry Visualization Server
# Copyright (c) 2018 Logan Barnes - All Rights Reserved
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
cmake_minimum_required(VERSION 3.12)
project(GeometryVisualizationServer LANGUAGES CXX)

option(GVS_BUILD_TESTS "Build unit tests" OFF)
option(GVS_USE_DEV_FLAGS "Compile with all the flags" OFF)
option(GVS_UPDATES_DISCONNECTED "Update third party repos" OFF)
option(GVS_BUILD_DOCS "Build documentation" OFF)

#############################
### Project Configuration ###
#############################
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()
message("-- Build type: ${CMAKE_BUILD_TYPE}")

if (MSVC)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # silly microsoft
    set(SYSTEM_FLAGS /experimental:external /external:anglebrackets /external:W0)
endif ()

if (GVS_USE_DEV_FLAGS)
    if (NOT MSVC)
        set(GVS_COMPILE_FLAGS -Wall -Wextra -Werror -Wpedantic -Winit-self
                -Wold-style-cast -Woverloaded-virtual -Wsign-conversion
                -Wmissing-declarations -Wmissing-include-dirs -Wshadow -Wno-unknown-pragmas)
    else ()
        set(GVS_COMPILE_FLAGS /WX)
    endif ()
endif ()

# "Glob is terrible/root of all evil" yeah yeah. CONFIGURE_DEPENDS in cmake 3.12
# helps to fix that and it is super useful when refactoring
cmake_policy(SET CMP0009 NEW)

##################
### Clang-Tidy ###
##################
find_program(CLANG_TIDY_EXE
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable"
        )
if (NOT CLANG_TIDY_EXE)
    message(STATUS "clang-tidy not found.")
else ()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-format-style=file" "-fix" "-fix-errors")
endif ()

#############################
### THIRD PARTY LIBRARIES ###
#############################
# Add Cuda if available
include(CheckLanguage)
check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
else ()
    message(STATUS "-- No CUDA support")
endif ()

add_subdirectory(third-party)

###########################
### Protobuf Generation ###
###########################
add_subdirectory(protos)

##################
### ProtoScene ###
##################
add_subdirectory(src)

####################
### Clang-Format ###
####################
# only format things if this is the parent directory
get_directory_property(has_parent PARENT_DIRECTORY)
if (NOT has_parent)
    find_program(CLANG_FORMAT_EXE
            NAMES "clang-format"
            DOC "Path to clang-format executable"
            )
    if (NOT CLANG_FORMAT_EXE)
        message(STATUS "clang-format not found.")
    else ()
        add_custom_target(format_everything
                COMMAND ./format.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                )
        add_dependencies(gvs_protos format_everything)
    endif ()
endif ()

#####################
### Documentation ###
#####################

if (GVS_BUILD_DOCS)
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_LIST_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target(doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)
    else ()
        message("Doxygen needs to be installed to generate the doxygen documentation")
    endif ()

endif ()