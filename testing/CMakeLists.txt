##########################################################################################
# Copyright (c) 2018 Logan Barnes - All Rights Reserved
# Geometry Visualisation Server
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
target_link_libraries(test_util PUBLIC gvs_common testing_protos)
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
            PUBLIC gmock_main
            )
    set_target_properties(${target_to_test}_tests PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

    target_sources(all_proto_scene_tests PUBLIC ${TEST_UTIL_SOURCE_FILES})
    target_link_libraries(all_proto_scene_tests PUBLIC ${target_to_test})
endfunction()

add_test_executable(gvs_common ${CMAKE_CURRENT_LIST_DIR}/common)
add_test_executable(gvs_server ${CMAKE_CURRENT_LIST_DIR}/server)
add_test_executable(vis_client ${CMAKE_CURRENT_LIST_DIR}/vis-client)
