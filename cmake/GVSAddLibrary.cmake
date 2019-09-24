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
function(gvs_add_library target cxx_standard)
    # Add the library with custom compile flags and link the testing library
    add_library(${target} ${ARGN})
    target_compile_options(${target} PRIVATE ${GVS_COMPILE_FLAGS})
    target_link_libraries(${target} PUBLIC doctest)

    if (GVS_BUILD_TESTS) # BUILDING WITH TESTS
        # Create an executable to run the tests
        set(test_target ${target}_tests)
        add_executable(${test_target} ${CMAKE_BINARY_DIR}/generated/null.cpp)
        target_link_libraries(${test_target}
                PRIVATE doctest_with_main
                PRIVATE ${target}
                )
        add_test(NAME test_${target} COMMAND ${test_target})

        if (COMMAND SETUP_TARGET_FOR_COVERAGE)
            # Create a build that generates coverage reports
            setup_target_for_coverage(${test_target}_coverage ${test_target} coverage)
            apply_coverage_dependencies(${target})
        endif ()

    else () # NOT BUILDING WITH TESTS
        # The testing library is linked but we disable all the test macros
        target_compile_definitions(${target} PRIVATE -DDOCTEST_CONFIG_DISABLE)
    endif ()

    set_target_properties(
            ${target}
            ${test_target}
            PROPERTIES
            # C++ flags
            CXX_STANDARD ${cxx_standard}
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            POSITION_INDEPENDENT_CODE ON
            # Clang-Tidy
            CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
            # Binary locations
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endfunction()