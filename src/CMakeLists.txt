##########################################################################################
# Copyright (c) 2018 Logan Barnes - All Rights Reserved
# Geometry Visualisation Server
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

###################
### Executables ###
###################
add_executable(vis_client_and_server exec/visualiser_client_and_server.cpp)
target_link_libraries(vis_client_and_server vis_client gvs_server)
set_target_properties(vis_client_and_server PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

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