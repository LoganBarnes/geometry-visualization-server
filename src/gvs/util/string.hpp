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

#include <string>

namespace gvs {
namespace util {

inline bool starts_with(const std::string& str, const std::string& prefix) {
    return str.compare(0, prefix.length(), prefix) == 0;
}

#ifdef DOCTEST_LIBRARY_INCLUDED
TEST_CASE("[gvs-util] string_starts_with") {
    CHECK(starts_with("", ""));
    CHECK(starts_with("check for prefix", ""));
    CHECK(starts_with("check for prefix", "c"));
    CHECK(starts_with("check for prefix", "check"));
    CHECK(starts_with("check for prefix", "check "));
    CHECK(starts_with("check for prefix", "check for prefix"));

    CHECK_FALSE(starts_with("", "?"));
    CHECK_FALSE(starts_with("check for prefix", "z"));
    CHECK_FALSE(starts_with("check for prefix", "checkk"));
    CHECK_FALSE(starts_with("check for prefix", "check  "));
    CHECK_FALSE(starts_with("check for prefix", "check for prefix?"));
}
#endif

} // namespace util
} // namespace gvs
