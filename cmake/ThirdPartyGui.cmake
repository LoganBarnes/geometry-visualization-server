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
include(FetchContent)

# All GUI thirdparty libraries put into a single target
add_library(gvs_gui_thirdparty INTERFACE)

### GLFW ###
FetchContent_Declare(glfw_dl
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3
        )

FetchContent_GetProperties(glfw_dl)
if (NOT glfw_dl_POPULATED)
    FetchContent_Populate(glfw_dl)

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(${glfw_dl_SOURCE_DIR} ${glfw_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

### ImGui ###
FetchContent_Declare(imgui_dl
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.71
        )

FetchContent_GetProperties(imgui_dl)
if (NOT imgui_dl_POPULATED)
    FetchContent_Populate(imgui_dl)
endif ()

### Corrade ###
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

FetchContent_Declare(corrade_dl
        GIT_REPOSITORY https://github.com/mosra/corrade.git
        GIT_TAG v2019.01
        )

FetchContent_GetProperties(corrade_dl)
if (NOT corrade_dl_POPULATED)
    FetchContent_Populate(corrade_dl)
    add_subdirectory(${corrade_dl_SOURCE_DIR} ${corrade_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

### Magnum ###
FetchContent_Declare(magnum_dl
        GIT_REPOSITORY https://github.com/mosra/magnum.git
        GIT_TAG v2019.01
        )

FetchContent_GetProperties(magnum_dl)
if (NOT magnum_dl_POPULATED)
    FetchContent_Populate(magnum_dl)

    set(BUILD_DEPRECATED OFF CACHE BOOL "" FORCE)
    set(WITH_EGLCONTEXT ON CACHE BOOL "" FORCE)
    set(WITH_GLFWAPPLICATION ON CACHE BOOL "" FORCE)
    set(WITH_WINDOWLESSEGLAPPLICATION ON CACHE BOOL "" FORCE)

    add_subdirectory(${magnum_dl_SOURCE_DIR} ${magnum_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

### Magnum Integration ###
FetchContent_Declare(magnum_integration_dl
        GIT_REPOSITORY https://github.com/mosra/magnum-integration.git
        GIT_TAG v2019.01
        )

FetchContent_GetProperties(magnum_integration_dl)
if (NOT magnum_integration_dl_POPULATED)
    FetchContent_Populate(magnum_integration_dl)

    set(WITH_IMGUI ON CACHE BOOL "" FORCE)
    set(IMGUI_DIR "${imgui_dl_SOURCE_DIR}" CACHE STRING "" FORCE)

    add_subdirectory(${magnum_integration_dl_SOURCE_DIR} ${magnum_integration_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

    # Add the cmake config files
    list(APPEND CMAKE_MODULE_PATH ${magnum_integration_dl_SOURCE_DIR}/modules)
endif ()

find_package(Magnum REQUIRED EglContext GL GlfwApplication WindowlessEglApplication)
find_package(MagnumIntegration REQUIRED ImGui)

# Set the include directory as system headers to avoid compiler warnings
target_include_directories(gvs_gui_thirdparty SYSTEM INTERFACE ${corrade_dl_SOURCE_DIR}/src)
target_include_directories(gvs_gui_thirdparty SYSTEM INTERFACE ${magnum_dl_SOURCE_DIR}/src)

# Add the necessary libraries
target_link_libraries(gvs_gui_thirdparty INTERFACE
        Corrade::Utility
        Magnum::Application
        Magnum::EglContext
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::Primitives
        Magnum::SceneGraph
        Magnum::Shaders
        Magnum::Trade
        Magnum::WindowlessEglApplication
        MagnumIntegration::ImGui
        )

