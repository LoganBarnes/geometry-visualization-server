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
#include "generic_guard.hpp"

// external
#include <doctest/doctest.h>

// standard
#include <string>

namespace {

bool push_called = false;
bool pop_called  = false;

[[maybe_unused]] auto void_push() {
    push_called = true;
}
[[maybe_unused]] auto void_pop() {
    pop_called = true;
}

[[maybe_unused]] auto float_push(float) {
    push_called = true;
}
[[maybe_unused]] auto float_pop(float) {
    pop_called = true;
}

[[maybe_unused]] auto string_push(const std::string&) {
    push_called = true;
}
[[maybe_unused]] auto string_pop(const std::string&) {
    pop_called = true;
}

[[maybe_unused]] auto int_and_string_push(int, const std::string&) {
    push_called = true;
}
[[maybe_unused]] auto int_and_string_pop(int, const std::string&) {
    pop_called = true;
}

} // namespace

TEST_CASE("[util] generic_guard") {
    using namespace gvs;

    push_called = false;
    pop_called  = false;

    SUBCASE("No lambdas") {

        SUBCASE("void push, void pop") {
            auto guard = util::make_guard(void_push, void_pop);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("float push, void pop") {
            auto guard = util::make_guard(float_push, void_pop, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("void push, float pop") {
            auto guard = util::make_guard(void_push, float_pop, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("float push, float pop") {
            auto guard = util::make_guard(float_push, float_pop, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("string push, void pop") {
            auto guard = util::make_guard(string_push, void_pop, "Will this compile?");
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("void push, string pop") {
            auto guard = util::make_guard(void_push, string_pop, "Of course it will compile!");
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("string push, string pop") {
            auto guard = util::make_guard(string_push, string_pop, std::string("This will too!"));
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("int and string push, void pop") {
            auto guard = util::make_guard(int_and_string_push, void_pop, 3, "Multi-arg?");
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("void push, int and string pop") {
            auto guard = util::make_guard(void_push, int_and_string_pop, 7, "");
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("int and string push, int and string pop") {
            auto guard = util::make_guard(int_and_string_push, int_and_string_pop, 21, std::string());
            CHECK(push_called);
            CHECK(!pop_called);
        }
    }

    SUBCASE("With lambdas") {

        SUBCASE("lambda push, void pop") {
            auto guard = util::make_guard([] { void_push(); }, void_pop);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("lambda push, float pop") {
            auto guard = util::make_guard([] { float_push(0.f); }, float_pop, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("void push, lamda pop") {
            auto guard = util::make_guard(void_push, [] { float_pop(0.f); });
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("float push, lambda pop") {
            auto guard = util::make_guard(
                float_push, [] { void_pop(); }, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("lambda push, lambda pop") {
            auto guard = util::make_guard([] { float_push(0.f); }, [] { int_and_string_pop(73, "blarg"); });
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("float lambda push, float lambda pop") {
            auto guard = util::make_guard([](float arg) { float_push(arg); }, [](float arg) { float_pop(arg); }, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }
    }

    CHECK(push_called);
    CHECK(pop_called);
}