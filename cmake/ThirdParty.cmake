##########################################################################################
# Copyright (c) 2019 Logan Barnes - All Rights Reserved
##########################################################################################
include(FetchContent)

# All GUI thirdparty libraries put into a single target
add_library(trid_thirdparty INTERFACE)

### Expected ###
FetchContent_Declare(
        expected_dl
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG v1.0.0
)

FetchContent_GetProperties(expected_dl)
if (NOT expected_dl_POPULATED)
    FetchContent_Populate(expected_dl)

    set(EXPECTED_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(EXPECTED_ENABLE_DOCS OFF CACHE BOOL "" FORCE)

    # compile with current project
    add_subdirectory(${expected_dl_SOURCE_DIR} ${expected_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif (NOT expected_dl_POPULATED)

### gl3w ###
FetchContent_Declare(
        gl3w_dl
        GIT_REPOSITORY https://github.com/skaslev/gl3w.git
        GIT_TAG master
)

FetchContent_GetProperties(gl3w_dl)
if (NOT gl3w_dl_POPULATED)
    FetchContent_Populate(gl3w_dl)

    # add command to create the header and source files
    add_custom_command(
            OUTPUT
            "${gl3w_dl_BINARY_DIR}/src/gl3w.c"
            "${gl3w_dl_BINARY_DIR}/include/GL/gl3w.h"
            "${gl3w_dl_BINARY_DIR}/include/GL/glcorearb.h"
            COMMAND python ${gl3w_dl_SOURCE_DIR}/gl3w_gen.py
            DEPENDS ${gl3w_dl_SOURCE_DIR}/gl3w_gen.py
            WORKING_DIRECTORY ${gl3w_dl_BINARY_DIR}
    )

    add_library(gl3w
            ${gl3w_dl_BINARY_DIR}/src/gl3w.c
            ${gl3w_dl_BINARY_DIR}/include/GL/gl3w.h
            ${gl3w_dl_BINARY_DIR}/include/GL/glcorearb.h
            )

    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)

    target_include_directories(gl3w SYSTEM PUBLIC ${gl3w_dl_BINARY_DIR}/include)
    target_link_libraries(gl3w ${CMAKE_DL_LIBS})
    target_link_libraries(gl3w OpenGL::GL)
endif (NOT gl3w_dl_POPULATED)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Quick
        QuickWidgets
        QuickControls2
        Qml
        REQUIRED
        )

### Expected ###
FetchContent_Declare(
        qt_advanced_docking_dl
        GIT_REPOSITORY https://github.com/LoganBarnes/Qt-Advanced-Docking-System.git
        GIT_TAG master
)

FetchContent_GetProperties(qt_advanced_docking_dl)
if (NOT qt_advanced_docking_dl_POPULATED)
    FetchContent_Populate(qt_advanced_docking_dl)

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    # compile with current project
    add_subdirectory(${qt_advanced_docking_dl_SOURCE_DIR} ${qt_advanced_docking_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif (NOT qt_advanced_docking_dl_POPULATED)
