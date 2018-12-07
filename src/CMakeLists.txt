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
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(AUTO_GENERATION_MESSAGE "***This file is auto generated. If you modify it the changes will be overwritten.***")
configure_file(${CMAKE_CURRENT_LIST_DIR}/gvs_paths.hpp.in ${CMAKE_BINARY_DIR}/gvs/gvs_paths.hpp)

############
### Util ###
############
add_library(gvs_util INTERFACE)
target_include_directories(gvs_util
        INTERFACE ${CMAKE_CURRENT_LIST_DIR}
        INTERFACE ${CMAKE_BINARY_DIR}
        )

##############
### Common ###
##############
file(GLOB_RECURSE GVS_COMMON_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/gvs/net/*
        )

add_library(gvs_net ${GVS_COMMON_SOURCE_FILES})

target_link_libraries(gvs_net
        PUBLIC gvs_protos
        PUBLIC gvs_util
        PUBLIC crossguid
        )

set_target_properties(gvs_net PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
target_compile_options(gvs_net PUBLIC ${GVS_COMPILE_FLAGS})

##############
### Server ###
##############
file(GLOB_RECURSE SCENE_SERVER_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/gvs/server/*
        )

add_library(gvs_server ${SCENE_SERVER_SOURCE_FILES})

target_link_libraries(gvs_server PUBLIC gvs_net)

set_target_properties(gvs_server PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

##################
### Vis Client ###
##################
file(GLOB_RECURSE VIS_CLIENT_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/gvs/vis-client/*
        )

add_library(vis_client ${VIS_CLIENT_SOURCE_FILES})

target_link_libraries(vis_client
        PUBLIC gvs_gui_thirdparty
        PUBLIC gvs_net
        )
set_target_properties(vis_client PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

##################
### Log Client ###
##################
file(GLOB_RECURSE LOG_CLIENT_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/gvs/log/*
        )

add_library(log_client ${LOG_CLIENT_SOURCE_FILES})
target_link_libraries(log_client PUBLIC gvs_net)
set_target_properties(log_client PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

###################
### Executables ###
###################
add_executable(vis_client_and_server exec/visualiser_client_and_server.cpp)
target_link_libraries(vis_client_and_server vis_client gvs_server)

add_executable(scene_server exec/scene_server.cpp)
target_link_libraries(scene_server gvs_server)

add_executable(visualiser_client exec/visualiser_client.cpp)
target_link_libraries(visualiser_client vis_client)

add_executable(message_client exec/message_client.cpp)
target_link_libraries(message_client log_client)

add_executable(log_test exec/log_test.cpp)
target_link_libraries(log_test log_client)

set_target_properties(vis_client_and_server scene_server visualiser_client PROPERTIES
        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

###############
### Testing ###
###############
if (${GVS_BUILD_TESTS})
    # add test coverage capabilities
    find_program(LCOV_EXE
            NAMES "lcov"
            DOC "Path to lcov executable"
            )

    if (LCOV_EXE AND CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE MATCHES "Debug")
        include(CodeCoverage)
        setup_target_for_coverage(gvs_coverage all_proto_scene_tests coverage)
        set(PROFILE_FLAGS --coverage -fprofile-arcs -ftest-coverage -fno-inline)

        target_compile_options(gvs_net PUBLIC ${PROFILE_FLAGS})
        target_compile_options(gvs_server PUBLIC ${PROFILE_FLAGS})
        target_compile_options(vis_client PUBLIC ${PROFILE_FLAGS})

        target_link_libraries(gvs_net PUBLIC gcov)
        target_link_libraries(gvs_server PUBLIC gcov)
        target_link_libraries(vis_client PUBLIC gcov)
    endif ()
endif ()