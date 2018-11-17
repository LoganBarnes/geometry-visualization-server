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
#include "gvs/util/callback_handler.hpp"
#include <gmock/gmock.h>

namespace {

TEST(CallbackHandlerTests, simple_callback) {
    auto add = [](int a, int b) { return a + b; };
    std::unique_ptr<gvs::util::CallbackInterface<int>> callback = gvs::util::make_callback<int>(add, 5, 7);

    ASSERT_EQ(callback->invoke(), 12);
}

TEST(CallbackHandlerTests, callback_with_additional_arguments) {
    auto add = [](int a, int b, int c) { return a + b + c; };
    std::unique_ptr<gvs::util::CallbackInterface<int, int>> callback = gvs::util::make_callback<int, int>(add, 5, 7);

    ASSERT_EQ(callback->invoke(-12), 0);
}

class Adder {
public:
    int add2(int a, int b) { return a + b; }
    int add3(int a, int b, int c) { return a + b + c; }
};

TEST(CallbackHandlerTests, member_function_callback) {
    Adder adder;
    std::unique_ptr<gvs::util::CallbackInterface<int>> callback
        = gvs::util::make_callback<int>(&Adder::add2, &adder, 5, 7);

    ASSERT_EQ(callback->invoke(), 12);
}

TEST(CallbackHandlerTests, member_function_callback_with_additional_arguments) {
    Adder adder;
    std::unique_ptr<gvs::util::CallbackInterface<int, int>> callback
        = gvs::util::make_callback<int, int>(&Adder::add3, &adder, 5, 7);

    ASSERT_EQ(callback->invoke(-12), 0);
}

} // namespace
