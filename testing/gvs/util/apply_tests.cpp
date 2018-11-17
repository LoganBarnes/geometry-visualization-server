// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2018 Logan Barnes - All Rights Reserved
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
#include "gvs/util/apply.hpp"

#include <gmock/gmock.h>

namespace {

TEST(ApplyTests, apply) {
    auto add = [](int a, int b) { return a + b; };
    auto args = std::make_tuple(5, 7);

    ASSERT_EQ(gvs::util::apply(add, args), 12);
}

TEST(ApplyTests, apply_additional_arguments) {
    auto add = [](int a, int b, int c) { return a + b + c; };
    auto args = std::make_tuple(5, 7);

    ASSERT_EQ(gvs::util::apply(add, args, -12), 0);
}

class Adder {
public:
    int add2(int a, int b) { return a + b; }
    int add3(int a, int b, int c) { return a + b + c; }
};

TEST(ApplyTests, apply_member_function) {
    Adder adder;
    auto args = std::make_tuple(&adder, 5, 7);

    ASSERT_EQ(gvs::util::apply(&Adder::add2, args), 12);
}

TEST(ApplyTests, apply_member_function_with_additional_arguments) {
    Adder adder;
    auto args = std::make_tuple(&adder, 5, 7);

    int ref = -12;
    ASSERT_EQ(gvs::util::apply(&Adder::add3, args, ref), 0);
}

} // namespace
