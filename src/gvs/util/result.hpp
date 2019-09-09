// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2019 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#if __cplusplus == 201703L
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

// gvs
#include "gvs/util/error.hpp"

// third-party
#include <tl/expected.hpp>

namespace gvs {
namespace util {

/**
 * @brief A type of tl::expected that requires the result to be used.
 *
 * Example usage:
 *
 * @code{.cpp}

    // function that returns 'MyType' or 'util::Error'
    util::Result<MyType> my_function(int non_negative) {
        if (non_negative < 0) {
            return tl::make_unexpected(MAKE_ERROR("number is negative"));
        }
        return non_negative; // Success
    }

    int main() {
        my_function(0); // compilation error since the return value is not used (C++17)

        auto result1 = my_function(1); // compilation error since 'result1' is unused

        auto result2 = my_function(2); // OK, result is used
        if (result2) {
            ...
        } else {
            ...
        }

        my_function(3).map(...).map_error(...); // OK

        util::ignore(my_function(4)); // OK, but not recommended

        auto result3 = my_function(5); // OK, but not recommended
        util::ignore(result3);

        return 0;
    }

 * @endcode
 *
 */
template <class T, class E>
struct __attribute__((warn_unused)) Result;

template <typename T, typename Error = util::Error>
struct NODISCARD Result : tl::expected<T, Error> {
    using tl::expected<T, Error>::expected;
};

template <typename... Args>
void ignore(Args&&...) {}

} // namespace util
} // namespace gvs
