##########################################################################################
# Copyright (c) 2018 Logan Barnes - All Rights Reserved
# Geometry Visualisation Server
##########################################################################################
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ${GVS_UPDATES_DISCONNECTED} CACHE BOOL "" FORCE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# All GUI thirdparty libraries put into a single target
add_library(gvs_gui_thirdparty INTERFACE)

###############
### Corrade ###
###############
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

FetchContent_Declare(corrade_dl
        GIT_REPOSITORY https://github.com/mosra/corrade.git
        GIT_TAG v2018.10
        )

FetchContent_GetProperties(corrade_dl)
if (NOT corrade_dl_POPULATED)
    FetchContent_Populate(corrade_dl)
    add_subdirectory(${corrade_dl_SOURCE_DIR} ${corrade_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    # Set the include directory as system headers to avoid compiler warnings
    target_include_directories(gvs_gui_thirdparty SYSTEM INTERFACE ${corrade_dl_SOURCE_DIR}/src)
endif ()

############
### GLFW ###
############
FetchContent_Declare(glfw_dl
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG master
        )

FetchContent_GetProperties(glfw_dl)
if (NOT glfw_dl_POPULATED)
    FetchContent_Populate(glfw_dl)

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(${glfw_dl_SOURCE_DIR} ${glfw_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

##############
### Magnum ###
##############
FetchContent_Declare(magnum_dl
        GIT_REPOSITORY https://github.com/mosra/magnum.git
        GIT_TAG v2018.10
        )

FetchContent_GetProperties(magnum_dl)
if (NOT magnum_dl_POPULATED)
    FetchContent_Populate(magnum_dl)

    set(WITH_GLFWAPPLICATION ON CACHE BOOL "" FORCE)
    add_subdirectory(${magnum_dl_SOURCE_DIR} ${magnum_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
    target_compile_options(MagnumGlfwApplication PRIVATE -w) # ignore thirdparty compilation warnings

    # Set the include directory as system headers to avoid compiler warnings
    target_include_directories(gvs_gui_thirdparty SYSTEM INTERFACE ${magnum_dl_SOURCE_DIR}/src)

    # Add the necessary libraries
    target_link_libraries(gvs_gui_thirdparty
            INTERFACE Magnum::Application
            INTERFACE Magnum::Magnum
            )

    # Add the FindMagnum and FindCorrade cmake files
    list(APPEND CMAKE_MODULE_PATH ${magnum_dl_SOURCE_DIR}/modules)
endif ()

find_package(Magnum REQUIRED GL GlfwApplication)

##############
### ImGui ###
##############
FetchContent_Declare(imgui_dl
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.65
        )

FetchContent_GetProperties(imgui_dl)
if (NOT imgui_dl_POPULATED)
    FetchContent_Populate(imgui_dl)

    add_library(imgui
            ${imgui_dl_SOURCE_DIR}/imconfig.h
            ${imgui_dl_SOURCE_DIR}/imgui.cpp
            ${imgui_dl_SOURCE_DIR}/imgui.h
            ${imgui_dl_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_dl_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_dl_SOURCE_DIR}/imgui_internal.h
            ${imgui_dl_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_dl_SOURCE_DIR}/imstb_rectpack.h
            ${imgui_dl_SOURCE_DIR}/imstb_textedit.h
            ${imgui_dl_SOURCE_DIR}/imstb_truetype.h
            ${imgui_dl_SOURCE_DIR}/examples/imgui_impl_glfw.h
            ${imgui_dl_SOURCE_DIR}/examples/imgui_impl_glfw.cpp
            ${imgui_dl_SOURCE_DIR}/examples/imgui_impl_opengl3.h
            ${imgui_dl_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp
            pseudo-header/GL/gl3w.h # fake gl3w header
            )
    target_include_directories(imgui SYSTEM PUBLIC
            ${imgui_dl_SOURCE_DIR}
            ${imgui_dl_SOURCE_DIR}/examples
            pseudo-header
            )
    target_link_libraries(imgui
            PUBLIC glfw
            PUBLIC Magnum::GL
            )
    target_compile_options(imgui PRIVATE -w) # ignore thirdparty compilation warnings
    target_link_libraries(gvs_gui_thirdparty INTERFACE imgui)
endif ()

##################
### GoogleTest ###
##################
if (${GVS_BUILD_TESTS})
    FetchContent_Declare(googletest_dl
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.8.1
            )

    FetchContent_GetProperties(googletest_dl)
    if (NOT googletest_dl_POPULATED)
        FetchContent_Populate(googletest_dl)

        # Prevent overriding the parent project's compiler/linker settings on Windows
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

        # compile googletest with current project
        add_subdirectory(${googletest_dl_SOURCE_DIR} ${googletest_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
        target_compile_options(gtest PRIVATE -w)
        target_compile_options(gtest_main PRIVATE -w)
        target_compile_options(gmock PRIVATE -w)
        target_compile_options(gmock_main PRIVATE -w)
    endif ()
endif ()

# make cmake modules available to the rest of the project
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
