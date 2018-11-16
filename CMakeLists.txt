##########################################################################################
# Copyright (c) 2018 Logan Barnes - All Rights Reserved
# Geometry Visualisation Server
##########################################################################################
cmake_minimum_required(VERSION 3.12)
project(GeometryVisualizationServer)

option(GVS_BUILD_TESTS "Build unit tests" OFF)
option(GVS_USE_DEV_FLAGS "Compile with all the flags" OFF)
option(GVS_UPDATES_DISCONNECTED "Update third party repos" OFF)

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
                -Wmissing-declarations -Wmissing-include-dirs -Wshadow)
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
add_subdirectory(third-party)

###########################
### Protobuf Generation ###
###########################
add_subdirectory(protos)

##################
### ProtoScene ###
##################
add_subdirectory(src)

###############
### Testing ###
###############
if (${GVS_BUILD_TESTS})
    add_subdirectory(testing)
endif ()

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
