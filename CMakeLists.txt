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
cmake_minimum_required(VERSION 3.12)
project(GeometryVisualizationServer LANGUAGES CXX)

option(GVS_NO_DISPLAY "Only build the gvs_scene_client library and dependencies" OFF)
option(GVS_BUILD_TESTS "Build unit tests" OFF)
option(GVS_BUILD_EXAMPLES "Build example programs" OFF)
option(GVS_USE_DEV_FLAGS "Compile with all the flags" OFF)

#############################
### Project Configuration ###
#############################
if (MSVC)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # silly microsoft
endif ()

if (GVS_USE_DEV_FLAGS)
    if (NOT MSVC)
        set(GVS_COMPILE_FLAGS
                $<$<COMPILE_LANGUAGE:CXX>:-Wall>
                $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
                $<$<COMPILE_LANGUAGE:CXX>:-Werror>
                $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
                $<$<COMPILE_LANGUAGE:CXX>:-Wunused>
                $<$<COMPILE_LANGUAGE:CXX>:-pedantic-errors>
                $<$<COMPILE_LANGUAGE:CXX>:-Winit-self>
                $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
                $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
                $<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>
                $<$<COMPILE_LANGUAGE:CXX>:-Wshadow>
                $<$<COMPILE_LANGUAGE:CXX>:-Wmissing-declarations>
                $<$<COMPILE_LANGUAGE:CXX>:-Wmissing-include-dirs>
                $<$<COMPILE_LANGUAGE:CXX>:-Wno-unknown-pragmas>
                )
    else ()
        set(GVS_COMPILE_FLAGS
                $<$<COMPILE_LANGUAGE:CXX>:/WX>
                $<$<COMPILE_LANGUAGE:CXX>:/experimental:external>
                $<$<COMPILE_LANGUAGE:CXX>:/external:anglebrackets>
                $<$<COMPILE_LANGUAGE:CXX>:/external:W0>
                )
    endif ()
endif ()

include(cmake/CCache.cmake)
include(cmake/ClangTidy.cmake)
include(cmake/ThirdParty.cmake)
if (NOT GVS_NO_DISPLAY)
    include(cmake/ThirdPartyGui.cmake)
endif ()
include(cmake/GVSAddLibrary.cmake)
include(cmake/GVSAddExecutable.cmake)

if (GVS_BUILD_TESTS)
    enable_testing()
endif ()

# "Glob is terrible/root of all evil" yeah yeah. CONFIGURE_DEPENDS in cmake 3.12
# helps to fix that and it is super useful when refactoring
cmake_policy(SET CMP0009 NEW)

set(AUTO_GENERATION_MESSAGE "***This file is auto generated. If you modify it the changes will be overwritten.***")
configure_file(${CMAKE_CURRENT_LIST_DIR}/src/gvs_paths.hpp.in ${CMAKE_BINARY_DIR}/generated/gvs/gvs_paths.hpp)

################################
### gRPC/Protobuf Generation ###
################################
create_proto_library(gvs_protos
        ${CMAKE_CURRENT_LIST_DIR}/protos/gvs
        ${CMAKE_BINARY_DIR}/generated/protos
        )
target_compile_options(gvs_protos PRIVATE -w) # ignore warning on generated headers
set_target_properties(
        gvs_protos
        PROPERTIES
        # C++ flags
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        # Binary locations
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

####################
### Scene Client ###
####################
file(GLOB_RECURSE GVS_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/gvs/scene/*
        )

gvs_add_library(gvs_scene_client 11 ${GVS_SOURCE_FILES})
target_link_libraries(gvs_scene_client
        PUBLIC gvs_protos
        PUBLIC boostuuid
        )
target_include_directories(gvs_scene_client
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>"
        )

if (NOT GVS_NO_DISPLAY)
    ############
    ### Util ###
    ############
    file(GLOB_RECURSE GVS_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/src/gvs/util/*
            )

    gvs_add_library(gvs_util 17 ${GVS_SOURCE_FILES})

    target_link_libraries(gvs_util
            PUBLIC gvs_protos
            PUBLIC boostuuid
            PUBLIC expected
            PUBLIC doctest
            )
    target_include_directories(gvs_util
            PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
            PUBLIC ${CMAKE_BINARY_DIR}/generated
            )

    ###########
    ### GVS ###
    ###########
    # Compile resources separately to avoid linting issues on generated code
    corrade_add_resource(gvs_client_RESOURCES ${CMAKE_CURRENT_LIST_DIR}/src/resources.conf)
    add_library(vis_client_resources ${gvs_client_RESOURCES})
    target_link_libraries(vis_client_resources PUBLIC gvs_gui_thirdparty)

    #############
    ### GVS 2 ###
    #############
    file(GLOB_RECURSE GVS_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/src/gvs/gui/*
            ${CMAKE_CURRENT_LIST_DIR}/src/gvs/display/*
            ${CMAKE_CURRENT_LIST_DIR}/src/gvs/vis-client/*
            )

    gvs_add_library(gvs_display 17 ${GVS_SOURCE_FILES})
    target_link_libraries(gvs_display
            PUBLIC vis_client_resources
            PUBLIC gvs_util
            )
endif (NOT GVS_NO_DISPLAY)

if (GVS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()