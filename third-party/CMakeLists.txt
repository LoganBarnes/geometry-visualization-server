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
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ${GVS_UPDATES_DISCONNECTED} CACHE BOOL "" FORCE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

if (NOT GVS_LOGGING_ONLY)

    # All GUI thirdparty libraries put into a single target
    add_library(gvs_gui_thirdparty INTERFACE)

    ###############
    ### Corrade ###
    ###############
    set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

    FetchContent_Declare(corrade_gvs_dl
            GIT_REPOSITORY https://github.com/mosra/corrade.git
            GIT_TAG v2019.01
            )

    FetchContent_GetProperties(corrade_gvs_dl)
    if (NOT corrade_gvs_dl_POPULATED)
        FetchContent_Populate(corrade_gvs_dl)
        add_subdirectory(${corrade_gvs_dl_SOURCE_DIR} ${corrade_gvs_dl_BINARY_DIR} EXCLUDE_FROM_ALL)

        # Set the include directory as system headers to avoid compiler warnings
        target_include_directories(gvs_gui_thirdparty SYSTEM INTERFACE ${corrade_gvs_dl_SOURCE_DIR}/src)
    endif ()

    ############
    ### GLFW ###
    ############
    FetchContent_Declare(glfw_gvs_dl
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.3
            )

    FetchContent_GetProperties(glfw_gvs_dl)
    if (NOT glfw_gvs_dl_POPULATED)
        FetchContent_Populate(glfw_gvs_dl)

        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

        add_subdirectory(${glfw_gvs_dl_SOURCE_DIR} ${glfw_gvs_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()

    ##############
    ### Magnum ###
    ##############
    FetchContent_Declare(magnum_gvs_dl
            GIT_REPOSITORY https://github.com/mosra/magnum.git
            GIT_TAG v2019.01
            )

    FetchContent_GetProperties(magnum_gvs_dl)
    if (NOT magnum_gvs_dl_POPULATED)
        FetchContent_Populate(magnum_gvs_dl)

        set(WITH_GLFWAPPLICATION ON CACHE BOOL "" FORCE)
        set(BUILD_DEPRECATED OFF CACHE BOOL "" FORCE)

        add_subdirectory(${magnum_gvs_dl_SOURCE_DIR} ${magnum_gvs_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
        target_compile_options(MagnumGlfwApplication PRIVATE -w) # ignore thirdparty compilation warnings

        # Set the include directory as system headers to avoid compiler warnings
        target_include_directories(gvs_gui_thirdparty SYSTEM INTERFACE ${magnum_gvs_dl_SOURCE_DIR}/src)

        # Add the necessary libraries
        target_link_libraries(gvs_gui_thirdparty INTERFACE
                Corrade::Utility
                Magnum::Application
                Magnum::Magnum
                Magnum::MeshTools
                Magnum::Primitives
                Magnum::SceneGraph
                Magnum::Shaders
                Magnum::Trade
                )

        # Add the FindMagnum and FindCorrade cmake files
        list(APPEND CMAKE_MODULE_PATH ${magnum_gvs_dl_SOURCE_DIR}/modules)
    endif ()

    find_package(Magnum REQUIRED GL GlfwApplication)

    ##############
    ### ImGui ###
    ##############
    FetchContent_Declare(imgui_gvs_dl
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG v1.70
            )

    FetchContent_GetProperties(imgui_gvs_dl)
    if (NOT imgui_gvs_dl_POPULATED)
        FetchContent_Populate(imgui_gvs_dl)

        add_library(gvs_imgui
                ${imgui_gvs_dl_SOURCE_DIR}/imconfig.h
                ${imgui_gvs_dl_SOURCE_DIR}/imgui.cpp
                ${imgui_gvs_dl_SOURCE_DIR}/imgui.h
                ${imgui_gvs_dl_SOURCE_DIR}/imgui_demo.cpp
                ${imgui_gvs_dl_SOURCE_DIR}/imgui_draw.cpp
                ${imgui_gvs_dl_SOURCE_DIR}/imgui_internal.h
                ${imgui_gvs_dl_SOURCE_DIR}/imgui_widgets.cpp
                ${imgui_gvs_dl_SOURCE_DIR}/imstb_rectpack.h
                ${imgui_gvs_dl_SOURCE_DIR}/imstb_textedit.h
                ${imgui_gvs_dl_SOURCE_DIR}/imstb_truetype.h
                ${imgui_gvs_dl_SOURCE_DIR}/examples/imgui_impl_glfw.h
                ${imgui_gvs_dl_SOURCE_DIR}/examples/imgui_impl_glfw.cpp
                ${imgui_gvs_dl_SOURCE_DIR}/examples/imgui_impl_opengl3.h
                ${imgui_gvs_dl_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp
                pseudo-header/GL/gl3w.h # fake gl3w header
                )
        target_include_directories(gvs_imgui SYSTEM
                PUBLIC ${imgui_gvs_dl_SOURCE_DIR}
                PUBLIC ${imgui_gvs_dl_SOURCE_DIR}/examples
                PUBLIC pseudo-header
                )
        target_link_libraries(gvs_imgui
                PUBLIC glfw
                PUBLIC Magnum::GL
                )
        target_compile_options(gvs_imgui PRIVATE -w) # ignore thirdparty compilation warnings
        target_link_libraries(gvs_gui_thirdparty INTERFACE gvs_imgui)
    endif ()

endif (NOT GVS_LOGGING_ONLY)

# All non-GUI thirdparty libraries put into a single target
add_library(gvs_non_gui_thirdparty INTERFACE)

#################
### crossguid ###
#################
FetchContent_Declare(crossguid_gvs_dl
        GIT_REPOSITORY https://github.com/graeme-hill/crossguid.git
        GIT_TAG 0f2753174c914bc561504b14c963d179283fe829
        )

FetchContent_GetProperties(crossguid_gvs_dl)
if (NOT crossguid_gvs_dl_POPULATED)
    FetchContent_Populate(crossguid_gvs_dl)
    add_subdirectory(${crossguid_gvs_dl_SOURCE_DIR} ${crossguid_gvs_dl_BINARY_DIR} EXCLUDE_FROM_ALL)
    target_compile_options(crossguid PRIVATE -w)
    target_link_libraries(gvs_non_gui_thirdparty INTERFACE crossguid)
endif ()

###############
### Threads ###
###############
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(gvs_gui_thirdparty INTERFACE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(gvs_non_gui_thirdparty INTERFACE ${CMAKE_THREAD_LIBS_INIT})

#############
### OptiX ###
#############
if (NOT GVS_LOGGING_ONLY AND CMAKE_CUDA_COMPILER)
    find_package(OptiX)

    if (optix_LIBRARY AND OptiX_INCLUDE)
        message("-- OptiX found: ${optix_LIBRARY}")
        set(OptiX_FOUND TRUE PARENT_SCOPE)

        add_library(gvs_optix_thirdparty INTERFACE)

        target_link_libraries(gvs_optix_thirdparty INTERFACE optix optixu)
        target_include_directories(gvs_optix_thirdparty SYSTEM
                INTERFACE ${OptiX_INCLUDE}
                INTERFACE ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
                )

        function(cuda_convert_flags EXISTING_TARGET)
            get_property(old_flags TARGET ${EXISTING_TARGET} PROPERTY INTERFACE_COMPILE_OPTIONS)
            if (NOT "${old_flags}" STREQUAL "")
                string(REPLACE ";" "," CUDA_flags "${old_flags}")
                set_property(TARGET ${EXISTING_TARGET} PROPERTY INTERFACE_COMPILE_OPTIONS
                        "$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CXX>>:${old_flags}>$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=${CUDA_flags}>"
                        )
            endif ()
        endfunction()
    endif ()
endif ()

###############
### DocTest ###
###############
if (${GVS_BUILD_TESTS})
    FetchContent_Declare(doctest_gvs_dl
            GIT_REPOSITORY https://github.com/onqtam/doctest.git
            GIT_TAG 2.2.0
            )

    FetchContent_GetProperties(doctest_gvs_dl)
    if (NOT doctest_gvs_dl_POPULATED)
        FetchContent_Populate(doctest_gvs_dl)

        add_library(gvs_testing_thirdparty INTERFACE)
        target_include_directories(gvs_testing_thirdparty INTERFACE ${doctest_gvs_dl_SOURCE_DIR})
        target_link_libraries(gvs_testing_thirdparty INTERFACE Threads::Threads)
    endif ()
endif ()

# make cmake modules available to the rest of the project
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
