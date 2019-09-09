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

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

if (NOT ${GVS_LOGGING_ONLY})
    include(cmake/ThirdPartyGui.cmake)
endif ()

if (${GVS_BUILD_TESTS})
    include(cmake/ThirdPartyTesting.cmake)
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
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

############
### Util ###
############
file(GLOB_RECURSE GVS_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/gvs/util/*
        )

add_library(gvs_util ${GVS_SOURCE_FILES})

target_link_libraries(gvs_util
        PUBLIC gvs_protos
        PUBLIC crossguid
        PUBLIC expected
        )
target_include_directories(gvs_util
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
        PUBLIC ${CMAKE_BINARY_DIR}/generated
        )

target_compile_options(gvs_util PUBLIC ${GVS_COMPILE_FLAGS})

set_target_properties(
        gvs_util
        PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
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

add_library(gvs_log_client ${GVS_SOURCE_FILES})
target_link_libraries(gvs_log_client PUBLIC gvs_util)

set_target_properties(
        gvs_log_client
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if (NOT ${GVS_LOGGING_ONLY})
    ##############
    ### Server ###
    ##############
    file(GLOB_RECURSE GVS_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/src/gvs/server/*
            )

    add_library(gvs_server ${GVS_SOURCE_FILES})
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

    add_library(gvs_vis_client ${GVS_SOURCE_FILES})
    target_link_libraries(gvs_vis_client
            PUBLIC vis_client_resources
            PUBLIC gvs_util
            )

    ####################
    #### Executables ###
    ####################
    add_executable(run_gvs ${CMAKE_CURRENT_LIST_DIR}/src/exec/run_gvs.cpp)
    target_link_libraries(run_gvs
            PRIVATE gvs_server
            PRIVATE gvs_vis_client
            )

    add_executable(gvs_message_client ${CMAKE_CURRENT_LIST_DIR}/src/exec/message_client.cpp)
    target_link_libraries(gvs_message_client gvs_log_client)

    set_target_properties(
            gvs_server
            gvs_vis_client
            run_gvs
            gvs_message_client
            PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            POSITION_INDEPENDENT_CODE ON
            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )

    add_executable(gvs_log_test ${CMAKE_CURRENT_LIST_DIR}/src/exec/logging_examples/log_test.cpp)
    target_link_libraries(gvs_log_test gvs_log_client)

    add_executable(gvs_mesh_test ${CMAKE_CURRENT_LIST_DIR}/src/exec/logging_examples/mesh_test.cpp)
    target_link_libraries(gvs_mesh_test gvs_log_client)

    set_target_properties(
            gvs_log_test
            gvs_mesh_test
            PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            POSITION_INDEPENDENT_CODE ON
            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif (NOT ${GVS_LOGGING_ONLY})

################
#### Testing ###
################
if (${GVS_BUILD_TESTS})
    #    file(GLOB_RECURSE GVS_TEST_UTIL_SOURCE_FILES
    #            LIST_DIRECTORIES false
    #            CONFIGURE_DEPENDS
    #            ${CMAKE_CURRENT_LIST_DIR}/testing/util/*
    #            )
    #
    #    add_library(gvs_test_util ${GVS_TEST_UTIL_SOURCE_FILES})
    #
    #    target_link_libraries(gvs_test_util PUBLIC
    #            gvs_testing_thirdparty
    #            testing_protos
    #            gvs_net
    #            )
    #
    #    target_compile_options(gvs_test_util PUBLIC ${GVS_COMPILE_FLAGS})
    #
    #    # add test coverage capabilities
    #    find_program(LCOV_EXE
    #            NAMES "lcov"
    #            DOC "Path to lcov executable"
    #            )
    #
    #    if (LCOV_EXE AND CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE MATCHES "Debug")
    #        include(CodeCoverage)
    #        setup_target_for_coverage(gvs_coverage gvs_tests coverage)
    #        set(PROFILE_FLAGS --coverage -fprofile-arcs -ftest-coverage -fno-inline)
    #
    #        target_compile_options(gvs_net PUBLIC ${PROFILE_FLAGS})
    #        target_compile_options(gvs_server PUBLIC ${PROFILE_FLAGS})
    #        target_compile_options(gvs_vis_client PUBLIC ${PROFILE_FLAGS})
    #        target_compile_options(gvs_log_client PUBLIC ${PROFILE_FLAGS})
    #        target_compile_options(gvs_test_util PUBLIC ${PROFILE_FLAGS})
    #
    #        target_link_libraries(gvs_net PUBLIC gcov)
    #        target_link_libraries(gvs_server PUBLIC gcov)
    #        target_link_libraries(gvs_vis_client PUBLIC gcov)
    #        target_link_libraries(gvs_log_client PUBLIC gcov)
    #        target_link_libraries(gvs_test_util PUBLIC gcov)
    #    endif ()
    #
    #    add_executable(gvs_tests testing/gvs_tests.cpp)
    #    target_link_libraries(gvs_tests PRIVATE
    #            gvs_server
    #            gvs_vis_client
    #            gvs_log_client
    #            gvs_test_util
    #            )
    #
    #    set_target_properties(gvs_tests PROPERTIES
    #            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    #            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
    #            )
endif ()
