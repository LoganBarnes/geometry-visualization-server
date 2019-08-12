##########################################################################################
# Copyright (c) 2019 Logan Barnes - All Rights Reserved
##########################################################################################
cmake_minimum_required(VERSION 3.12)
project(TriDexel LANGUAGES C CXX CUDA)

option(TRID_BUILD_TESTS "Build unit tests" OFF)
option(TRID_USE_DEV_FLAGS "Compile with all the flags" OFF)

#############################
### Project Configuration ###
#############################
include(cmake/ClangTidy.cmake)
include(cmake/ThirdParty.cmake)

if (${TRID_BUILD_TESTS})
    include(cmake/ThirdPartyTesting.cmake)
endif ()

if (MSVC)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS) # silly microsoft
    set(SYSTEM_FLAGS /experimental:external /external:anglebrackets /external:W0)
endif ()

if (TRID_USE_DEV_FLAGS)
    if (NOT MSVC)
        set(TRID_COMPILE_FLAGS
                -Wall -Wextra -Werror -Wpedantic -Wunused -pedantic-errors -Winit-self
                -Wold-style-cast -Woverloaded-virtual -Wsign-conversion -Wshadow
                -Wmissing-declarations -Wmissing-include-dirs -Wno-unknown-pragmas
                )
    else ()
        set(TRID_COMPILE_FLAGS /WX)
    endif ()
endif ()

# "Glob is terrible/root of all evil" yeah yeah. CONFIGURE_DEPENDS in cmake 3.12
# helps to fix that and it is super useful when refactoring
cmake_policy(SET CMP0009 NEW)

set(AUTO_GENERATION_MESSAGE "***This file is auto generated. If you modify it the changes will be overwritten.***")
configure_file(${CMAKE_CURRENT_LIST_DIR}/src/paths.hpp.in ${CMAKE_BINARY_DIR}/generated/trid/paths.hpp)

###############
### Library ###
###############
file(GLOB_RECURSE TRID_RESOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/*.qrc
        )
file(GLOB_RECURSE TRID_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/trid/*
        ${CMAKE_CURRENT_LIST_DIR}/src/util/*
        ${CMAKE_CURRENT_LIST_DIR}/src/ui/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ui/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ui/*.ui
        )

add_library(tridexel
        ${TRID_RESOURCE_FILES}
        ${TRID_SOURCE_FILES}
        )

target_link_libraries(tridexel
        PUBLIC
        expected
        gl3w
        qtadvanceddocking
        Qt5::Core
        Qt5::Gui
        Qt5::Quick
        Qt5::QuickWidgets
        Qt5::QuickControls2
        Qt5::Qml
        )
target_include_directories(tridexel
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src;${CMAKE_BINARY_DIR}/generated>"
        )

target_compile_options(tridexel PRIVATE ${TRID_COMPILE_FLAGS})

##################
### Executable ###
##################
add_executable(tridexel_viewer
        ${TRID_RESOURCE_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/src/exec/tridexel_viewer.cpp
        )
target_link_libraries(tridexel_viewer PRIVATE tridexel)
target_compile_options(tridexel_viewer PRIVATE ${TRID_COMPILE_FLAGS})

#########################
### Target Properties ###
#########################
set_target_properties(
        tridexel
        tridexel_viewer
        PROPERTIES
        # C++ flags
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        # Binary locations
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        # Just Qt things
        INCLUDE_CURRENT_DIR ON # Find includes in corresponding build directories
        AUTOMOC ON # Instruct CMake to run moc automatically when needed
        AUTOUIC ON # Create code from a list of Qt designer ui files
        AUTORCC ON # Create code for resource loading
)
