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
configure_file(${CMAKE_CURRENT_LIST_DIR}/gvs_paths.hpp.in ${CMAKE_BINARY_DIR}/generated/gvs/gvs_paths.hpp)

############
### Util ###
############
add_library(gvs_util INTERFACE)
target_include_directories(gvs_util
        INTERFACE ${CMAKE_CURRENT_LIST_DIR}
        INTERFACE ${CMAKE_BINARY_DIR}/generated
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

target_compile_options(gvs_net PUBLIC ${GVS_COMPILE_FLAGS})

##############
### Server ###
##############
if (NOT GVS_LOGGING_ONLY)
    file(GLOB_RECURSE SCENE_SERVER_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/gvs/server/*
            )

    add_library(gvs_server ${SCENE_SERVER_SOURCE_FILES})

    target_link_libraries(gvs_server PUBLIC gvs_net)

    ##################
    ### Vis Client ###
    ##################
    # Compile resources separately to avoid linting issues on generated code
    corrade_add_resource(gvs_client_RESOURCES resources.conf)
    add_library(vis_client_resources ${gvs_client_RESOURCES})
    target_link_libraries(vis_client_resources PUBLIC gvs_gui_thirdparty)

    file(GLOB_RECURSE VIS_CLIENT_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/gvs/vis-client/*
            )

    add_library(gvs_vis_client ${VIS_CLIENT_SOURCE_FILES})

    target_link_libraries(gvs_vis_client
            PUBLIC vis_client_resources
            PUBLIC gvs_net
            )

    ######################
    ### Optional OptiX ###
    ######################
    if (${OptiX_FOUND})
        set(CMAKE_CUDA_STANDARD 14)
        set(CMAKE_CUDA_STANDARD_REQUIRED ON)

        ### OptiX PTX ###
        file(GLOB_RECURSE OPTIX_PTX_SOURCE_FILES
                LIST_DIRECTORIES false
                CONFIGURE_DEPENDS
                ${CMAKE_CURRENT_LIST_DIR}/gvs/optix/ptx/*
                )

        add_library(gvs_ptx OBJECT ${OPTIX_PTX_SOURCE_FILES})
        set_property(TARGET gvs_ptx PROPERTY CUDA_PTX_COMPILATION ON)
        target_include_directories(gvs_ptx PRIVATE .)
        target_link_libraries(gvs_ptx PUBLIC gvs_optix_thirdparty)

        file(GENERATE
                OUTPUT ${CMAKE_BINARY_DIR}/generated/gvs/optix/ptx_files.hpp
                CONTENT [[
#pragma once

namespace gvs {
namespace ptx {

constexpr const char* ptx_files = "$<TARGET_OBJECTS:gvs_ptx>";

} // namespace ptx
} // namespace gvs

]]
                )

        ### OptiX Scene ###
        file(GLOB_RECURSE OPTIX_SCENE_SOURCE_FILES
                LIST_DIRECTORIES false
                CONFIGURE_DEPENDS
                ${CMAKE_CURRENT_LIST_DIR}/gvs/optix/scene/*
                )

        add_library(gvs_optix_scene ${OPTIX_SCENE_SOURCE_FILES})
        add_dependencies(gvs_optix_scene gvs_ptx)
        target_link_libraries(gvs_optix_scene
                gvs_gui_thirdparty
                gvs_net
                gvs_optix_thirdparty
                )

        set_target_properties(gvs_optix_scene PROPERTIES CUDA_SEPERABLE_COMPILATION ON)

        cuda_convert_flags(gvs_ptx)
        cuda_convert_flags(gvs_optix_scene)

        # OBJECT_OUTPUT_DIRECTORY is wrong but figure it out.
        set_target_properties(gvs_ptx gvs_optix_scene PROPERTIES
                CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
                LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/ptx"
                )

        # link to gvs_vis_client
        target_link_libraries(gvs_vis_client PUBLIC gvs_optix_scene)
    endif ()

endif (NOT GVS_LOGGING_ONLY)

##################
### Log Client ###
##################
file(GLOB_RECURSE LOG_CLIENT_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/gvs/log/*
        )

add_library(gvs_log_client ${LOG_CLIENT_SOURCE_FILES})
target_link_libraries(gvs_log_client PUBLIC gvs_net)

###################
### Executables ###
###################
if (NOT GVS_LOGGING_ONLY)
    add_executable(gvs_vis_client_and_server exec/visualiser_client_and_server.cpp)
    target_link_libraries(gvs_vis_client_and_server gvs_vis_client gvs_server)

    add_executable(gvs_scene_server exec/scene_server.cpp)
    target_link_libraries(gvs_scene_server gvs_server)

    add_executable(gvs_visualiser_client exec/visualiser_client.cpp)
    target_link_libraries(gvs_visualiser_client gvs_vis_client)

    add_executable(gvs_message_client exec/message_client.cpp)
    target_link_libraries(gvs_message_client gvs_log_client)

    add_executable(gvs_log_test exec/logging_examples/log_test.cpp)
    target_link_libraries(gvs_log_test gvs_log_client)

    add_executable(gvs_mesh_test exec/logging_examples/mesh_test.cpp)
    target_link_libraries(gvs_mesh_test gvs_log_client)

    ### Clang-Tidy for all ###
    set_target_properties(
            gvs_net
            gvs_server
            gvs_vis_client
            gvs_vis_client_and_server
            gvs_scene_server
            gvs_visualiser_client
            gvs_message_client
            gvs_log_test
            gvs_mesh_test
            PROPERTIES
            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

endif (NOT GVS_LOGGING_ONLY)

### Clang-Tidy for all ###
set_target_properties(
        gvs_log_client
        PROPERTIES
        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

###############
### Testing ###
###############
if (${GVS_BUILD_TESTS})
    file(GLOB_RECURSE GVS_TEST_UTIL_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/testing/util/*
            )

    add_library(gvs_test_util ${GVS_TEST_UTIL_SOURCE_FILES})

    target_link_libraries(gvs_test_util PUBLIC
            gvs_testing_thirdparty
            testing_protos
            gvs_net
            )

    target_compile_options(gvs_test_util PUBLIC ${GVS_COMPILE_FLAGS})

    # add test coverage capabilities
    find_program(LCOV_EXE
            NAMES "lcov"
            DOC "Path to lcov executable"
            )

    if (LCOV_EXE AND CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE MATCHES "Debug")
        include(CodeCoverage)
        setup_target_for_coverage(gvs_coverage gvs_tests coverage)
        set(PROFILE_FLAGS --coverage -fprofile-arcs -ftest-coverage -fno-inline)

        target_compile_options(gvs_net PUBLIC ${PROFILE_FLAGS})
        target_compile_options(gvs_server PUBLIC ${PROFILE_FLAGS})
        target_compile_options(gvs_vis_client PUBLIC ${PROFILE_FLAGS})
        target_compile_options(gvs_log_client PUBLIC ${PROFILE_FLAGS})
        target_compile_options(gvs_test_util PUBLIC ${PROFILE_FLAGS})

        target_link_libraries(gvs_net PUBLIC gcov)
        target_link_libraries(gvs_server PUBLIC gcov)
        target_link_libraries(gvs_vis_client PUBLIC gcov)
        target_link_libraries(gvs_log_client PUBLIC gcov)
        target_link_libraries(gvs_test_util PUBLIC gcov)
    endif ()

    add_executable(gvs_tests testing/gvs_tests.cpp)
    target_link_libraries(gvs_tests PRIVATE
            gvs_server
            gvs_vis_client
            gvs_log_client
            gvs_test_util
            )

    set_target_properties(gvs_tests PROPERTIES
            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
            )
endif ()