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
cmake_minimum_required(VERSION 3.15)
project(GeometryVisualizationServerExternal)

### Project Configuration ###
include(FetchContent)

function(FetchContent_MakeAvailable_ExcludeFromAll name)
    # Check if population has already been performed
    FetchContent_GetProperties(${name})
    string(TOLOWER "${name}" lcName)
    if (NOT ${lcName}_POPULATED)
        # Fetch the content using previously declared details
        FetchContent_Populate(${name})

        # Bring the populated content into the build
        add_subdirectory(${${lcName}_SOURCE_DIR} ${${lcName}_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()
endfunction()

include(cmake/ThirdParty.cmake)

if (GVS_ENABLE_NETWORKING)
    include(cmake/ThirdPartyNetworking.cmake)
endif ()

if (GVS_ENABLE_DISPLAY)
    include(cmake/ThirdPartyDisplay.cmake)
endif ()
