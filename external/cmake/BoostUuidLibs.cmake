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
function(add_boost_library name)
    set(LIB_DL boost_${name}_dl)
    set(LIB boost_${name})

    FetchContent_Declare(
            ${LIB_DL}
            GIT_REPOSITORY https://github.com/boostorg/${name}.git
            GIT_TAG boost-1.72.0
    )

    FetchContent_GetProperties(${LIB_DL})
    if (NOT ${LIB_DL}_POPULATED)
        FetchContent_Populate(${LIB_DL})

        add_library(${LIB} INTERFACE)
        target_include_directories(${LIB} SYSTEM INTERFACE "$<BUILD_INTERFACE:${${LIB_DL}_SOURCE_DIR}/include>")
    endif ()

endfunction()

# All the boost libs needed to use the boost uuid "header only" library
add_boost_library(config)
add_boost_library(type_traits)
add_boost_library(lexical_cast)
add_boost_library(range)
add_boost_library(assert)
add_boost_library(iterator)
add_boost_library(mpl)
add_boost_library(preprocessor)
add_boost_library(static_assert)
add_boost_library(core)
add_boost_library(detail)
add_boost_library(concept_check)
add_boost_library(utility)
add_boost_library(throw_exception)
add_boost_library(numeric_conversion)
add_boost_library(integer)
add_boost_library(array)
add_boost_library(container)
add_boost_library(move)
add_boost_library(math)
add_boost_library(predef)
add_boost_library(io)
add_boost_library(random)
add_boost_library(tti)
add_boost_library(function_types)

add_boost_library(uuid)

target_link_libraries(boost_uuid INTERFACE
        boost_config
        boost_type_traits
        boost_lexical_cast
        boost_range
        boost_assert
        boost_iterator
        boost_mpl
        boost_preprocessor
        boost_static_assert
        boost_core
        boost_detail
        boost_concept_check
        boost_utility
        boost_throw_exception
        boost_numeric_conversion
        boost_integer
        boost_array
        boost_container
        boost_move
        boost_math
        boost_predef
        boost_io
        boost_random
        boost_tti
        boost_function_types
        )
