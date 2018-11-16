// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/common/apply.hpp"

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
