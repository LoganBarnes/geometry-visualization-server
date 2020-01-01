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
function(add_example _directory _cpp17_enabled)
    set(target run_${_directory}_example)

    file(GLOB_RECURSE GVS_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/common/*
            ${CMAKE_CURRENT_LIST_DIR}/${_directory}/*
            )

    add_executable(${target} ${GVS_SOURCE_FILES})

    if (_cpp17_enabled)
        set(CPP_STANDARD 17)
        target_link_libraries(${target} PUBLIC gvs_scene_display)
    else ()
        set(CPP_STANDARD 11)
        target_link_libraries(${target} PUBLIC gvs_scene_client)
    endif ()

    target_compile_options(${target} PRIVATE ${GVS_COMPILE_FLAGS})
    set_target_properties(
            ${target}
            PROPERTIES
            # C++ flags
            CXX_STANDARD ${CPP_STANDARD}
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            POSITION_INDEPENDENT_CODE ON
            # CCache
            COMPILER_LAUNCHER "${GVS_CCACHE_PROGRAM}"
            # Clang-Tidy
            CXX_CLANG_TIDY "${GVS_CLANG_TIDY}"
            # Binary locations
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )

endfunction()

if (GVS_ENABLE_NETWORKING)
    add_example(cpp11_client FALSE)
else ()
    message("-- (GVS_ENABLE_NETWORKING=OFF) Some examples will not be built: cpp11_client")
endif ()

if (GVS_ENABLE_DISPLAY)
    add_example(primitives TRUE)
    add_example(scene TRUE)
else ()
    message("-- (GVS_ENABLE_DISPLAY=OFF) Some examples will not be built: primitives scene")
endif ()
