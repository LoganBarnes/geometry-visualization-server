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

option(GVS_LOGGING_ONLY "Only build the logging library" OFF)
option(GVS_BUILD_TESTS "Build unit tests" OFF)
option(GVS_USE_DEV_FLAGS "Compile with all the flags" OFF)

#############################
### Project Configuration ###
#############################
include(cmake/ClangTidy.cmake)
include(cmake/ThirdParty.cmake)

if (NOT ${GVS_LOGGING_ONLY})
    include(cmake/ThirdPartyGui.cmake)
endif ()

if (MSVC)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # silly microsoft
    set(SYSTEM_FLAGS /experimental:external /external:anglebrackets /external:W0)
endif ()

if (GVS_USE_DEV_FLAGS)
    if (NOT MSVC)
        set(GVS_COMPILE_FLAGS
                -Wall -Wextra -Werror -Wpedantic -Wunused -pedantic-errors -Winit-self
                -Wold-style-cast -Woverloaded-virtual -Wsign-conversion -Wshadow
                -Wmissing-declarations -Wmissing-include-dirs -Wno-unknown-pragmas
                )
    else ()
        set(GVS_COMPILE_FLAGS /WX)
    endif ()
endif ()

include(cmake/GVSAddLibrary.cmake)
include(cmake/GVSAddExecutable.cmake)

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
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        # Binary locations
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

##################
### Log Client ###
##################
file(GLOB_RECURSE GVS_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/gvs/log/*
        )

gvs_add_library(gvs_log_client 11 ${GVS_SOURCE_FILES})
target_link_libraries(gvs_log_client
        PUBLIC gvs_protos
        PUBLIC crossguid
        )
target_include_directories(gvs_log_client
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
        )

if (NOT ${GVS_LOGGING_ONLY})
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
            PUBLIC crossguid
            PUBLIC expected
            )
    target_include_directories(gvs_util
            PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
            PUBLIC ${CMAKE_BINARY_DIR}/generated
            )

    ##############
    ### Server ###
    ##############
    file(GLOB_RECURSE GVS_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/src/gvs/server/*
            )

    gvs_add_library(gvs_server 17 ${GVS_SOURCE_FILES})
    target_link_libraries(gvs_server PUBLIC gvs_util)

    ##################
    ### Vis Client ###
    ##################
    # Compile resources separately to avoid linting issues on generated code
    corrade_add_resource(gvs_client_RESOURCES ${CMAKE_CURRENT_LIST_DIR}/src/resources.conf)
    add_library(vis_client_resources ${gvs_client_RESOURCES})
    target_link_libraries(vis_client_resources PUBLIC gvs_gui_thirdparty)

    file(GLOB_RECURSE GVS_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/src/gvs/vis-client/*
            )

    gvs_add_library(gvs_vis_client 17 ${GVS_SOURCE_FILES})
    target_link_libraries(gvs_vis_client
            PUBLIC vis_client_resources
            PUBLIC gvs_util
            )

    ####################
    #### Executables ###
    ####################
    gvs_add_executable(run_gvs 17 ${CMAKE_CURRENT_LIST_DIR}/src/exec/run_gvs.cpp)
    target_link_libraries(run_gvs
            PRIVATE gvs_server
            PRIVATE gvs_vis_client
            )

    # TODO: Create actual tests for these test executables
    #    add_executable(gvs_message_client ${CMAKE_CURRENT_LIST_DIR}/src/exec/message_client.cpp)
    #    target_link_libraries(gvs_message_client gvs_log_client)
    #
    #    add_executable(gvs_log_test ${CMAKE_CURRENT_LIST_DIR}/src/exec/logging_examples/log_test.cpp)
    #    target_link_libraries(gvs_log_test gvs_log_client)
    #
    #    add_executable(gvs_mesh_test ${CMAKE_CURRENT_LIST_DIR}/src/exec/logging_examples/mesh_test.cpp)
    #    target_link_libraries(gvs_mesh_test gvs_log_client)
    #
    #    add_executable(gvs_mesh_test_17 ${CMAKE_CURRENT_LIST_DIR}/src/exec/logging_examples/mesh_test_17.cpp)
    #    target_link_libraries(gvs_mesh_test_17 gvs_log_client)
    #
    #    set_target_properties(
    #            gvs_log_test
    #            gvs_mesh_test
    #            gvs_mesh_test_17
    #            PROPERTIES
    #            CXX_STANDARD 14
    #            CXX_STANDARD_REQUIRED ON
    #            CXX_EXTENSIONS OFF
    #            POSITION_INDEPENDENT_CODE ON
    #            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    #            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    #            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    #            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    #    )
    #    set_target_properties(
    #            gvs_mesh_test_17
    #            PROPERTIES
    #            CXX_STANDARD 17
    #    )
endif (NOT ${GVS_LOGGING_ONLY})
