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

#################
### Test Util ###
#################
file(GLOB_RECURSE TEST_UTIL_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/test-util/*
        )

add_library(test_util ${TEST_UTIL_SOURCE_FILES})
target_link_libraries(test_util PUBLIC gvs_net testing_protos)
target_include_directories(test_util PUBLIC ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(test_util PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
target_compile_options(test_util PUBLIC ${GVS_COMPILE_FLAGS})

add_executable(all_proto_scene_tests)
target_link_libraries(all_proto_scene_tests
        PUBLIC test_util
        PUBLIC gmock_main
        )
set_target_properties(all_proto_scene_tests PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

########################
### Test Executable ###
########################
function(add_test_executable target_to_test test_directory)
    file(GLOB_RECURSE TEST_UTIL_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${test_directory}/*
            )

    add_executable(${target_to_test}_tests ${TEST_UTIL_SOURCE_FILES})

    target_link_libraries(${target_to_test}_tests
            PUBLIC ${target_to_test}
            PUBLIC test_util
            PUBLIC gvs_testing_thirdparty
            )
    set_target_properties(${target_to_test}_tests PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

    target_sources(all_proto_scene_tests PUBLIC ${TEST_UTIL_SOURCE_FILES})
    target_link_libraries(all_proto_scene_tests PUBLIC ${target_to_test})
endfunction()

add_test_executable(gvs_net ${CMAKE_CURRENT_LIST_DIR}/gvs/net)
add_test_executable(gvs_server ${CMAKE_CURRENT_LIST_DIR}/gvs/server)
add_test_executable(gvs_util ${CMAKE_CURRENT_LIST_DIR}/gvs/util)
add_test_executable(vis_client ${CMAKE_CURRENT_LIST_DIR}/gvs/vis-client)
